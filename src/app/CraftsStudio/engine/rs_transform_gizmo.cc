#include "rs_transform_gizmo.h"
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPropPicker.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkCoordinate.h>
#include <QDebug>

TransformGizmo::TransformGizmo(QObject* parent)
    : QObject(parent)
{
    m_transform = vtkSmartPointer<vtkTransform>::New();
    m_picker = vtkSmartPointer<vtkPropPicker>::New();
    
    // 创建各种操纵器组件
    createTranslateGizmo();
    createRotateGizmo();
    createScaleGizmo();
}

TransformGizmo::~TransformGizmo()
{
    // 清理所有操纵器Actor
    for (auto actor : m_translateHandles) {
        actor->Delete();
    }
    for (auto actor : m_rotateHandles) {
        actor->Delete();
    }
    for (auto actor : m_scaleHandles) {
        actor->Delete();
    }
    if (m_uniformScaleHandle) {
        m_uniformScaleHandle->Delete();
    }
}

void TransformGizmo::setRenderer(vtkRenderer* renderer)
{
    m_renderer = renderer;
}

void TransformGizmo::setTarget(vtkActor* target)
{
    m_target = target;
    updatePosition();
}

void TransformGizmo::setTransformMode(int mode)
{
    m_currentMode = mode;
    
    // 根据模式显示/隐藏相应的操纵器
    switch (mode) {
    case 1: // 平移模式
        for (auto actor : m_translateHandles) actor->SetVisibility(1);
        for (auto actor : m_rotateHandles) actor->SetVisibility(0);
        for (auto actor : m_scaleHandles) actor->SetVisibility(0);
        if (m_uniformScaleHandle) m_uniformScaleHandle->SetVisibility(0);
        break;
    case 2: // 旋转模式
        for (auto actor : m_translateHandles) actor->SetVisibility(0);
        for (auto actor : m_rotateHandles) actor->SetVisibility(1);
        for (auto actor : m_scaleHandles) actor->SetVisibility(0);
        if (m_uniformScaleHandle) m_uniformScaleHandle->SetVisibility(0);
        break;
    case 3: // 缩放模式
        for (auto actor : m_translateHandles) actor->SetVisibility(0);
        for (auto actor : m_rotateHandles) actor->SetVisibility(0);
        for (auto actor : m_scaleHandles) actor->SetVisibility(1);
        if (m_uniformScaleHandle) m_uniformScaleHandle->SetVisibility(1);
        break;
    default: // 无模式
        for (auto actor : m_translateHandles) actor->SetVisibility(0);
        for (auto actor : m_rotateHandles) actor->SetVisibility(0);
        for (auto actor : m_scaleHandles) actor->SetVisibility(0);
        if (m_uniformScaleHandle) m_uniformScaleHandle->SetVisibility(0);
        break;
    }
}

void TransformGizmo::show()
{
    m_visible = true;
    setTransformMode(m_currentMode);
}

void TransformGizmo::hide()
{
    m_visible = false;
    for (auto actor : m_translateHandles) actor->SetVisibility(0);
    for (auto actor : m_rotateHandles) actor->SetVisibility(0);
    for (auto actor : m_scaleHandles) actor->SetVisibility(0);
    if (m_uniformScaleHandle) m_uniformScaleHandle->SetVisibility(0);
}

void TransformGizmo::updatePosition()
{
    if (!m_target) return;
    
    // 获取目标对象的边界框和位置
    double bounds[6];
    m_target->GetBounds(bounds);
    
    // 计算操纵器位置（目标对象的中心）
    double center[3] = {
        (bounds[0] + bounds[1]) / 2.0,
        (bounds[2] + bounds[3]) / 2.0,
        (bounds[4] + bounds[5]) / 2.0
    };
    
    // 计算操纵器大小（基于目标对象大小）
    double size = std::max({bounds[1]-bounds[0], bounds[3]-bounds[2], bounds[5]-bounds[4]}) * 0.3;
    
    // 更新变换矩阵
    m_transform->Identity();
    m_transform->Translate(center);
    
    // 更新所有操纵器位置
    for (auto actor : getGizmoActors()) {
        actor->SetPosition(center);
        actor->SetScale(size);
    }
}

QList<vtkActor*> TransformGizmo::getGizmoActors() const
{
    QList<vtkActor*> actors;
    actors.append(m_translateHandles);
    actors.append(m_rotateHandles);
    actors.append(m_scaleHandles);
    if (m_uniformScaleHandle) {
        actors.append(m_uniformScaleHandle);
    }
    return actors;
}

GizmoHandleType TransformGizmo::pickHandle(const double pickPoint[3])
{
    if (!m_visible) return GizmoHandleType::None;
    
    // 这里简化实现，实际应该使用射线投射进行精确检测
    // 临时返回一个默认手柄类型
    if (m_currentMode == 1) return GizmoHandleType::TranslateX;
    if (m_currentMode == 2) return GizmoHandleType::RotateX;
    if (m_currentMode == 3) return GizmoHandleType::ScaleX;
    
    return GizmoHandleType::None;
}

void TransformGizmo::translate(double delta[3])
{
    if (!m_target) return;
    
    // 应用平移变换
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    if (auto userTransform = m_target->GetUserTransform()) {
        transform->DeepCopy(userTransform);
    }
    if (!transform) {
        transform = vtkTransform::New();
        m_target->SetUserTransform(transform);
    }
    
    transform->Translate(delta);
    updatePosition();
    emit transformApplied();
}

void TransformGizmo::rotate(double angle, int axis)
{
    if (!m_target) return;
    
    // 应用旋转变换
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    if (auto userTransform = m_target->GetUserTransform()) {
        transform->DeepCopy(userTransform);
    }
    if (!transform) {
        transform = vtkTransform::New();
        m_target->SetUserTransform(transform);
    }
    
    switch (axis) {
    case 0: transform->RotateX(angle); break; // X轴
    case 1: transform->RotateY(angle); break; // Y轴
    case 2: transform->RotateZ(angle); break; // Z轴
    }
    
    updatePosition();
    emit transformApplied();
}

void TransformGizmo::scale(double factor, int axis)
{
    if (!m_target) return;
    
    // 应用缩放变换
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    if (auto userTransform = m_target->GetUserTransform()) {
        transform->DeepCopy(userTransform);
    }
    if (!transform) {
        transform = vtkTransform::New();
        m_target->SetUserTransform(transform);
    }
    
    double scaleFactors[3] = {1.0, 1.0, 1.0};
    if (axis == -1) {
        // 均匀缩放
        scaleFactors[0] = scaleFactors[1] = scaleFactors[2] = factor;
    } else {
        // 轴向缩放
        scaleFactors[axis] = factor;
    }
    
    transform->Scale(scaleFactors);
    updatePosition();
    emit transformApplied();
}

// 创建平移操纵器
void TransformGizmo::createTranslateGizmo()
{
    m_translateHandles.clear();
    
    // 创建X、Y、Z轴平移手柄
    double red[3] = {1.0, 0.0, 0.0};
    double green[3] = {0.0, 1.0, 0.0};
    double blue[3] = {0.0, 0.0, 1.0};
    m_translateHandles.append(createArrowHandle(0, red));    // X轴
    m_translateHandles.append(createArrowHandle(1, green));  // Y轴
    m_translateHandles.append(createArrowHandle(2, blue));   // Z轴
}

// 创建旋转操纵器
void TransformGizmo::createRotateGizmo()
{
    m_rotateHandles.clear();
    
    // 创建X、Y、Z轴旋转环
    double red[3] = {1.0, 0.0, 0.0};
    double green[3] = {0.0, 1.0, 0.0};
    double blue[3] = {0.0, 0.0, 1.0};
    m_rotateHandles.append(createRingHandle(0, red));    // X轴
    m_rotateHandles.append(createRingHandle(1, green));  // Y轴
    m_rotateHandles.append(createRingHandle(2, blue));   // Z轴
}

// 创建缩放操纵器
void TransformGizmo::createScaleGizmo()
{
    m_scaleHandles.clear();
    
    // 创建X、Y、Z轴缩放手柄
    double red[3] = {1.0, 0.0, 0.0};
    double green[3] = {0.0, 1.0, 0.0};
    double blue[3] = {0.0, 0.0, 1.0};
    m_scaleHandles.append(createCubeHandle(0, red));     // X轴
    m_scaleHandles.append(createCubeHandle(1, green));  // Y轴
    m_scaleHandles.append(createCubeHandle(2, blue));   // Z轴
    
    // 创建均匀缩放手柄
    m_uniformScaleHandle = createUniformScaleHandle();
}

// 创建箭头手柄（用于平移）
vtkActor* TransformGizmo::createArrowHandle(int axis, double color[3])
{
    // 创建箭头几何体
    auto coneSource = vtkSmartPointer<vtkConeSource>::New();
    coneSource->SetHeight(0.2);
    coneSource->SetRadius(0.05);
    coneSource->SetResolution(20);
    
    auto cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
    cylinderSource->SetHeight(0.6);
    cylinderSource->SetRadius(0.02);
    cylinderSource->SetResolution(20);
    
    // 根据轴线方向旋转
    auto transform = vtkSmartPointer<vtkTransform>::New();
    if (axis == 0) { // X轴
        transform->RotateY(90);
    } else if (axis == 1) { // Y轴
        transform->RotateX(-90);
    }
    // Z轴不需要旋转
    
    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetTransform(transform);
    
    // 这里简化实现，实际应该组合锥体和圆柱体
    transformFilter->SetInputConnection(cylinderSource->GetOutputPort());
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());
    
    auto actor = vtkActor::New();
    actor->SetMapper(mapper);
    setupHandleProperties(actor, color);
    
    return actor;
}

// 创建环形手柄（用于旋转）
vtkActor* TransformGizmo::createRingHandle(int axis, double color[3])
{
    auto tubeSource = vtkSmartPointer<vtkCylinderSource>::New();
    tubeSource->SetHeight(0.02);
    tubeSource->SetRadius(0.8);
    tubeSource->SetResolution(50);
    
    // 根据轴线方向旋转
    auto transform = vtkSmartPointer<vtkTransform>::New();
    if (axis == 0) { // X轴
        transform->RotateY(90);
    } else if (axis == 1) { // Y轴
        transform->RotateX(-90);
    }
    // Z轴不需要旋转
    
    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetTransform(transform);
    transformFilter->SetInputConnection(tubeSource->GetOutputPort());
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());
    
    auto actor = vtkActor::New();
    actor->SetMapper(mapper);
    setupHandleProperties(actor, color);
    
    return actor;
}

// 创建立方体手柄（用于缩放）
vtkActor* TransformGizmo::createCubeHandle(int axis, double color[3])
{
    auto cubeSource = vtkSmartPointer<vtkCubeSource>::New();
    cubeSource->SetXLength(0.1);
    cubeSource->SetYLength(0.1);
    cubeSource->SetZLength(0.1);
    
    // 根据轴线方向定位
    auto transform = vtkSmartPointer<vtkTransform>::New();
    if (axis == 0) { // X轴
        transform->Translate(0.8, 0, 0);
    } else if (axis == 1) { // Y轴
        transform->Translate(0, 0.8, 0);
    } else if (axis == 2) { // Z轴
        transform->Translate(0, 0, 0.8);
    }
    
    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetTransform(transform);
    transformFilter->SetInputConnection(cubeSource->GetOutputPort());
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());
    
    auto actor = vtkActor::New();
    actor->SetMapper(mapper);
    setupHandleProperties(actor, color);
    
    return actor;
}

// 创建均匀缩放手柄
vtkActor* TransformGizmo::createUniformScaleHandle()
{
    auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(0.15);
    sphereSource->SetThetaResolution(20);
    sphereSource->SetPhiResolution(20);
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    
    auto actor = vtkActor::New();
    actor->SetMapper(mapper);
    double yellow[3] = {1.0, 1.0, 0.0};
    setupHandleProperties(actor, yellow);
    
    return actor;
}

// 设置手柄属性
void TransformGizmo::setupHandleProperties(vtkActor* actor, double color[3])
{
    auto property = actor->GetProperty();
    property->SetColor(color);
    property->SetAmbient(0.3);
    property->SetDiffuse(0.7);
    property->SetSpecular(0.1);
    property->SetLineWidth(2.0);
    
    // 设置手柄为透明，只在鼠标悬停时高亮
    property->SetOpacity(0.7);
    
    actor->SetVisibility(0); // 默认隐藏
}

// 视觉反馈功能实现
void TransformGizmo::highlightHandle(GizmoHandleType handleType, int axisIndex)
{
    if (m_isHandleHighlighted && m_highlightedHandle == handleType && m_highlightedAxis == axisIndex) {
        return; // 已经是当前高亮的手柄
    }
    
    // 清除之前的高亮
    clearHighlight();
    
    // 高亮新的手柄
    vtkActor* handleToHighlight = nullptr;
    
    // 根据手柄类型和轴向找到对应的Actor
    switch (handleType) {
    case GizmoHandleType::TranslateX:
    case GizmoHandleType::TranslateY:
    case GizmoHandleType::TranslateZ:
        if (axisIndex >= 0 && axisIndex < m_translateHandles.size()) {
            handleToHighlight = m_translateHandles[axisIndex];
        }
        break;
    case GizmoHandleType::RotateX:
    case GizmoHandleType::RotateY:
    case GizmoHandleType::RotateZ:
        if (axisIndex >= 0 && axisIndex < m_rotateHandles.size()) {
            handleToHighlight = m_rotateHandles[axisIndex];
        }
        break;
    case GizmoHandleType::ScaleX:
    case GizmoHandleType::ScaleY:
    case GizmoHandleType::ScaleZ:
        if (axisIndex >= 0 && axisIndex < m_scaleHandles.size()) {
            handleToHighlight = m_scaleHandles[axisIndex];
        }
        break;
    case GizmoHandleType::UniformScale:
        handleToHighlight = m_uniformScaleHandle;
        break;
    default:
        break;
    }
    
    if (handleToHighlight) {
        // 设置高亮属性
        auto property = handleToHighlight->GetProperty();
        property->SetColor(1.0, 1.0, 0.0); // 黄色高亮
        property->SetOpacity(1.0);        // 完全不透明
        property->SetAmbient(0.6);       // 增加环境光
        property->SetDiffuse(0.8);        // 增加漫反射
        
        m_isHandleHighlighted = true;
        m_highlightedHandle = handleType;
        m_highlightedAxis = axisIndex;
        
        // 如果渲染器存在，触发重绘
        if (m_renderer) {
            m_renderer->GetRenderWindow()->Render();
        }
    }
}

void TransformGizmo::clearHighlight()
{
    if (!m_isHandleHighlighted) return;
    
    // 恢复所有手柄的原始颜色
    for (auto actor : m_translateHandles) {
        double color[3] = {1.0, 0.0, 0.0};
        if (m_translateHandles.indexOf(actor) == 1) color[1] = 1.0; // Y轴绿色
        if (m_translateHandles.indexOf(actor) == 2) color[2] = 1.0; // Z轴蓝色
        setupHandleProperties(actor, color);
    }
    
    for (auto actor : m_rotateHandles) {
        double color[3] = {1.0, 0.0, 0.0};
        if (m_rotateHandles.indexOf(actor) == 1) color[1] = 1.0;
        if (m_rotateHandles.indexOf(actor) == 2) color[2] = 1.0;
        setupHandleProperties(actor, color);
    }
    
    for (auto actor : m_scaleHandles) {
        double color[3] = {1.0, 0.0, 0.0};
        if (m_scaleHandles.indexOf(actor) == 1) color[1] = 1.0;
        if (m_scaleHandles.indexOf(actor) == 2) color[2] = 1.0;
        setupHandleProperties(actor, color);
    }
    
    if (m_uniformScaleHandle) {
        double yellow[3] = {1.0, 1.0, 0.0};
        setupHandleProperties(m_uniformScaleHandle, yellow);
    }
    
    m_isHandleHighlighted = false;
    m_highlightedHandle = GizmoHandleType::None;
    m_highlightedAxis = -1;
    
    // 如果渲染器存在，触发重绘
    if (m_renderer) {
        m_renderer->GetRenderWindow()->Render();
    }
}

void TransformGizmo::startTransformPreview()
{
    if (!m_target || m_isTransforming) return;
    
    m_isTransforming = true;
    
    // 保存原始变换
    m_originalTransform = vtkSmartPointer<vtkTransform>::New();
    if (auto userTransform = m_target->GetUserTransform()) {
        m_originalTransform->DeepCopy(userTransform);
    }
    
    // 创建预览Actor（半透明副本）
    if (m_target) {
        m_previewActor = vtkActor::New();
        if (auto mapper = m_target->GetMapper()) {
            m_previewActor->SetMapper(mapper);
        }
        
        // 设置预览属性
        auto property = m_previewActor->GetProperty();
        property->SetColor(0.5, 0.5, 1.0); // 蓝色半透明
        property->SetOpacity(0.5);
        property->SetAmbient(0.3);
        property->SetDiffuse(0.7);
        
        // 添加到渲染器
        if (m_renderer) {
            m_renderer->AddActor(m_previewActor);
        }
    }
    
    // 重置预览变换数据
    m_previewTransform[0] = m_previewTransform[1] = m_previewTransform[2] = 0.0;
}

void TransformGizmo::updateTransformPreview(double dx, double dy, int axisIndex, GizmoHandleType handleType)
{
    if (!m_isTransforming || !m_target || !m_previewActor) return;
    
    // 根据手柄类型和轴向计算变换
    double sensitivity = 0.01; // 变换灵敏度
    
    switch (handleType) {
    case GizmoHandleType::TranslateX:
        m_previewTransform[0] = dx * sensitivity;
        break;
    case GizmoHandleType::TranslateY:
        m_previewTransform[1] = -dy * sensitivity;
        break;
    case GizmoHandleType::TranslateZ:
        m_previewTransform[2] = dy * sensitivity;
        break;
    case GizmoHandleType::RotateX:
    case GizmoHandleType::RotateY:
    case GizmoHandleType::RotateZ:
        m_previewTransform[0] = dx * 0.5; // 旋转角度
        break;
    case GizmoHandleType::ScaleX:
    case GizmoHandleType::ScaleY:
    case GizmoHandleType::ScaleZ:
        m_previewTransform[0] = 1.0 + dy * 0.01; // 缩放因子
        break;
    case GizmoHandleType::UniformScale:
        m_previewTransform[0] = 1.0 + dy * 0.01;
        m_previewTransform[1] = m_previewTransform[0];
        m_previewTransform[2] = m_previewTransform[0];
        break;
    default:
        break;
    }
    
    // 应用预览变换到预览Actor
    if (m_previewActor) {
        auto transform = vtkSmartPointer<vtkTransform>::New();
        if (m_originalTransform) {
            transform->DeepCopy(m_originalTransform);
        }
        
        // 根据手柄类型应用变换
        if (handleType >= GizmoHandleType::TranslateX && handleType <= GizmoHandleType::TranslateZ) {
            transform->Translate(m_previewTransform);
        }
        else if (handleType >= GizmoHandleType::RotateX && handleType <= GizmoHandleType::RotateZ) {
            transform->RotateX(m_previewTransform[0] * (axisIndex == 0 ? 1.0 : 0.0));
            transform->RotateY(m_previewTransform[0] * (axisIndex == 1 ? 1.0 : 0.0));
            transform->RotateZ(m_previewTransform[0] * (axisIndex == 2 ? 1.0 : 0.0));
        }
        else if (handleType >= GizmoHandleType::ScaleX && handleType <= GizmoHandleType::UniformScale) {
            if (handleType == GizmoHandleType::UniformScale) {
                transform->Scale(m_previewTransform[0], m_previewTransform[1], m_previewTransform[2]);
            } else {
                double scaleFactors[3] = {1.0, 1.0, 1.0};
                scaleFactors[axisIndex] = m_previewTransform[0];
                transform->Scale(scaleFactors);
            }
        }
        
        m_previewActor->SetUserTransform(transform);
    }
    
    // 更新渲染
    if (m_renderer) {
        m_renderer->GetRenderWindow()->Render();
    }
}

void TransformGizmo::endTransformPreview()
{
    if (!m_isTransforming) return;
    
    m_isTransforming = false;
    
    // 移除预览Actor
    if (m_previewActor && m_renderer) {
        m_renderer->RemoveActor(m_previewActor);
        m_previewActor->Delete();
        m_previewActor = nullptr;
    }
    
    // 清理状态
    m_originalTransform = nullptr;
    m_previewTransform[0] = m_previewTransform[1] = m_previewTransform[2] = 0.0;
    
    // 更新渲染
    if (m_renderer) {
        m_renderer->GetRenderWindow()->Render();
    }
}

// 新增函数实现
bool TransformGizmo::PickHandle(int x, int y, int& handleType, int& axisIndex)
{
    // 简化实现：基于当前模式返回手柄类型
    if (!m_visible) return false;
    
    handleType = m_currentMode - 1; // 0=平移, 1=旋转, 2=缩放
    axisIndex = 0; // 默认X轴
    
    return true;
}

void TransformGizmo::ApplyTranslation(double dx, double dy, int axisIndex)
{
    if (!m_target) return;
    
    // 根据鼠标移动计算平移向量
    double delta[3] = {0.0, 0.0, 0.0};
    double scale = 0.01; // 平移灵敏度
    
    switch (axisIndex) {
    case 0: // X轴
        delta[0] = dx * scale;
        break;
    case 1: // Y轴
        delta[1] = -dy * scale;
        break;
    case 2: // Z轴
        delta[2] = dy * scale;
        break;
    }
    
    translate(delta);
}

void TransformGizmo::ApplyRotation(double dx, double dy, int axisIndex)
{
    if (!m_target) return;
    
    // 根据鼠标移动计算旋转角度
    double angle = dx * 0.5; // 旋转灵敏度
    
    rotate(angle, axisIndex);
}

void TransformGizmo::ApplyScale(double dx, double dy, int axisIndex)
{
    if (!m_target) return;
    
    // 根据鼠标移动计算缩放因子
    double factor = 1.0 + dy * 0.01; // 缩放灵敏度
    
    if (axisIndex == 3) {
        // 均匀缩放
        scale(factor, -1);
    } else {
        // 轴向缩放
        scale(factor, axisIndex);
    }
}

void TransformGizmo::SetTargetObject(vtkActor* target)
{
    setTarget(target);
}

void TransformGizmo::SetMode(int mode)
{
    setTransformMode(mode);
}

void TransformGizmo::Show()
{
    show();
}

void TransformGizmo::Hide()
{
    hide();
}

// 精确的射线拾取检测
bool TransformGizmo::rayPickHandle(int x, int y, GizmoHandleType& handleType, int& axisIndex)
{
  if (!m_visible || !m_renderer) {
    handleType = GizmoHandleType::None;
    axisIndex = -1;
    return false;
  }

  // 配置拾取器
  m_picker->PickFromListOn();
  
  // 根据当前模式添加相应的手柄到拾取列表
  m_picker->InitializePickList();
  
  switch (m_currentMode) {
  case 1: // 平移模式
    for (auto actor : m_translateHandles) {
      m_picker->AddPickList(actor);
    }
    break;
  case 2: // 旋转模式
    for (auto actor : m_rotateHandles) {
      m_picker->AddPickList(actor);
    }
    break;
  case 3: // 缩放模式
    for (auto actor : m_scaleHandles) {
      m_picker->AddPickList(actor);
    }
    if (m_uniformScaleHandle) {
      m_picker->AddPickList(m_uniformScaleHandle);
    }
    break;
  default:
    handleType = GizmoHandleType::None;
    axisIndex = -1;
    return false;
  }

  // 执行拾取，设置适当的容差
  // 注意：vtkPropPicker 没有 SetTolerance 方法，使用默认容差即可
  
  // 尝试多次拾取以获得最佳结果
  vtkActor* bestActor = nullptr;
  double bestDistance = std::numeric_limits<double>::max();
  
  // 多次拾取提高精度
  for (int attempt = 0; attempt < 3; ++attempt) {
    if (m_picker->Pick(x, y, 0, m_renderer)) {
      vtkActor* pickedActor = m_picker->GetActor();
      if (pickedActor) {
        double pickPos[3];
        m_picker->GetPickPosition(pickPos);
        
        // 计算拾取点到相机距离
        double cameraPos[3];
        m_renderer->GetActiveCamera()->GetPosition(cameraPos);
        double distance = vtkMath::Distance2BetweenPoints(pickPos, cameraPos);
        
        // 选择距离最近的手柄（通常在前面）
        if (distance < bestDistance) {
          bestActor = pickedActor;
          bestDistance = distance;
        }
      }
    }
  }
  
  if (bestActor) {
    // 确定拾取的手柄类型和轴向
    if (m_currentMode == 1) { // 平移模式
      if (m_translateHandles.contains(bestActor)) {
        int index = m_translateHandles.indexOf(bestActor);
        axisIndex = index;
        switch (index) {
        case 0: handleType = GizmoHandleType::TranslateX; break;
        case 1: handleType = GizmoHandleType::TranslateY; break;
        case 2: handleType = GizmoHandleType::TranslateZ; break;
        default: handleType = GizmoHandleType::None; break;
        }
        return true;
      }
    }
    else if (m_currentMode == 2) { // 旋转模式
      if (m_rotateHandles.contains(bestActor)) {
        int index = m_rotateHandles.indexOf(bestActor);
        axisIndex = index;
        switch (index) {
        case 0: handleType = GizmoHandleType::RotateX; break;
        case 1: handleType = GizmoHandleType::RotateY; break;
        case 2: handleType = GizmoHandleType::RotateZ; break;
        default: handleType = GizmoHandleType::None; break;
        }
        return true;
      }
    }
    else if (m_currentMode == 3) { // 缩放模式
      if (m_scaleHandles.contains(bestActor)) {
        int index = m_scaleHandles.indexOf(bestActor);
        axisIndex = index;
        switch (index) {
        case 0: handleType = GizmoHandleType::ScaleX; break;
        case 1: handleType = GizmoHandleType::ScaleY; break;
        case 2: handleType = GizmoHandleType::ScaleZ; break;
        default: handleType = GizmoHandleType::None; break;
        }
        return true;
      }
      else if (bestActor == m_uniformScaleHandle) {
        handleType = GizmoHandleType::UniformScale;
        axisIndex = 3; // 均匀缩放的特殊标识
        return true;
      }
    }
  }

  // 如果多重拾取失败，尝试传统拾取
  if (m_picker->Pick(x, y, 0, m_renderer)) {
    vtkActor* pickedActor = m_picker->GetActor();
    if (!pickedActor) {
      handleType = GizmoHandleType::None;
      axisIndex = -1;
      return false;
    }

    // 确定拾取的手柄类型和轴向
    if (m_currentMode == 1) { // 平移模式
      if (m_translateHandles.contains(pickedActor)) {
        int index = m_translateHandles.indexOf(pickedActor);
        axisIndex = index;
        switch (index) {
        case 0: handleType = GizmoHandleType::TranslateX; break;
        case 1: handleType = GizmoHandleType::TranslateY; break;
        case 2: handleType = GizmoHandleType::TranslateZ; break;
        default: handleType = GizmoHandleType::None; break;
        }
        return true;
      }
    }
    else if (m_currentMode == 2) { // 旋转模式
      if (m_rotateHandles.contains(pickedActor)) {
        int index = m_rotateHandles.indexOf(pickedActor);
        axisIndex = index;
        switch (index) {
        case 0: handleType = GizmoHandleType::RotateX; break;
        case 1: handleType = GizmoHandleType::RotateY; break;
        case 2: handleType = GizmoHandleType::RotateZ; break;
        default: handleType = GizmoHandleType::None; break;
        }
        return true;
      }
    }
    else if (m_currentMode == 3) { // 缩放模式
      if (m_scaleHandles.contains(pickedActor)) {
        int index = m_scaleHandles.indexOf(pickedActor);
        axisIndex = index;
        switch (index) {
        case 0: handleType = GizmoHandleType::ScaleX; break;
        case 1: handleType = GizmoHandleType::ScaleY; break;
        case 2: handleType = GizmoHandleType::ScaleZ; break;
        default: handleType = GizmoHandleType::None; break;
        }
        return true;
      }
      else if (pickedActor == m_uniformScaleHandle) {
        handleType = GizmoHandleType::UniformScale;
        axisIndex = 3; // 均匀缩放的特殊标识
        return true;
      }
    }
  }

  handleType = GizmoHandleType::None;
  axisIndex = -1;
  return false;
}