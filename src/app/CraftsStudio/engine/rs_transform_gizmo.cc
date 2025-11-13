#include "rs_transform_gizmo.h"
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <QDebug>

TransformGizmo::TransformGizmo(QObject* parent)
    : QObject(parent)
{
    m_transform = vtkSmartPointer<vtkTransform>::New();
    
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