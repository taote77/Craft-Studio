#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H

namespace heygears {

namespace defines {

enum ErrorCode : int {
    NO_ERROR = 0,

    //PRINT_ERROR
    PRINT_OPEN_MODEL = 1000,
    PRINT_LOAD_TECHBAG,
    PRINT_BAD_PARAMS,
    PRINT_MODEL_PARSER,
    PRINT_PROCESSPACKAGE_PARSER,
    PRINT_FIND_TECHBAG,
    PRINT_TECHBAG_MISMATCH,
    PRINT_CONVER_PARAMS,
    PRINT_IS_PRINTING,
    PRINT_CREATE_THREAD,
    PRINT_ZEROING,
    PRINT_PREPRECESS,
    PRINT_EXPOSURE,
    PRINT_PEEL, // unused
    PRINT_WAIT,
    PRINT_RESET,
    PRINT_NULL_HANDLE,
    PRINT_INTERRUPT = 1017,
    PRINT_LOAD_CACHE,
    PRINT_SET_ENERGY,
    PRINT_PEEL_UP,
    PRINT_PEEL_DOWN,
    PRINT_MAGNET_SWITCH,
    PRINT_EXPOSURE_TIME,
    PRINT_LED_ON,
    PRINT_LED_OFF,
    PRINT_SCREEN_CLEAR,
    PRINT_RESIN_LEVEL_LOAD = 1027, //读取液位失败
    PRINT_RESIN_TIMEOUT,           //<加液超时
    PRINT_RESIN_REMAIN,            //< 树脂余量不足
    PRINT_FILLING_ABNORMAL,        //< 加液异常
    PRINT_FILLING_RUNNING,         //< 加液线程运行中
    PRINT_VALVE_CTRL,              //< 阀门控制失败
    PRINT_RESIN_UNRESPONSIVE,      //< 加液模块未响应

    /* 中间预留与前端错误码冲突 */
    PRINT_HEAT_CHECK_FORCE_WARN  = 1045,    // 打印前加热力值告警
    PRINT_HEAT_RESIN_TEMP_TOO_LOW,          // 加热树脂温度过低 1046
    PRINT_HEAT_TIMEOUT,                     // 加热超时 1047
    PRINT_HEAT_FAILED,                      // 加热失败 1048
    PRINT_HEAT_ENV_TEMP_SENSOR_ABNORMAL=1050, // 打印前加热红外传感器异常 1050
    PRINT_HEAT_ZERO_FAILED=1051,            // 打印前加热寻零失败 1051
    PRINT_HEAT_RESIN_TEMP_LOW,              // 加热树脂温度低 1052
    PRINT_BOARD_DETECTION,                  // 掉板检测
    PRINT_COLLISION_DETECTION,              // 检测到震动
    PRINT_PROCESS_LAYER_PARAM = 1055,      //< 层数参数错误
    PRINT_PROCESS_EXPOSURE_PARAM,          //< 曝光参数错误
    PRINT_PROCESS_UP_DISTANCE_PARAM,       //< 上升参数错误
    PRINT_PROCESS_DOWN_DISTANCE_PARAM,     //< 下降参数错误
    PRINT_PROCESS_UP_SPEED_PARAM,          //< 上升速度参数错误
    PRINT_PROCESS_DOWN_SPEED_PARAM,        //< 下降速度参数错误
    PRINT_PROCESS_WAIT_PARAM,              //< 等待参数错误
    PRINT_PROCESS_OPENCV,                  //< OPENCV处理失败
    PRINT_PROCESS_STD,                     //< STD标准处理失败
    PRINT_PROCESS_READ_IMAGE,              //< 读取图像失败
    PRINT_PROCESS_SUPPORT_IMAGE,           //< 支撑图片处理错误
    PRINT_PROCESS_OBJECT_IMAGE,            //< 实体图片处理错误
    PRINT_CHECK_FORCE_WARN,                //< 力值检查警告
    PRINT_SET_FASTPEEL_FAIL,               //< 快速剥离配置失败
    PRINT_PROCESS_UDISK_DISCONNECT = 1069, //< U盘断开连接
    PRINT_PROCESS_SPACE_NOT_ENOUGH,        //< 解压空间不足
    PRINT_ZAXIS_FORCE_SENSOR_ABNORMAL,     //< 主轴力传感器异常
    PRINT_SET_AUTOWAIT_PARAM_ERROR,        //< 设置自适应策略参数错误
    PRINT_PROCESS_IMGAE_SIZE,              //< 图像尺寸(分辨率)错误
    PRINT_WARN_BOTH_FORCE_AND_RESIN,       //< 同时上报力值警告和液位警告
    PRINT_SCRAPER_CONNECT_ERROR,            //< 刮刀连接异常
    PRINT_SCRAPER_LIMIT_ERROR,              //< 刮刀限位异常(未运动到限位,堵转)
    PRINT_HEAT_PARAM_ERROR,                //< 加热参数异常
    PRINT_HEAT_RESIN_TEMP_SENSOR_ABNORMAL, //< 加热树脂温度传感器异常
    PRINT_SCRAPER_MOVE_FAIL,               //< 刮刀运动失败 (堵转复位失败上报这个错误码)
    PRINT_SCRAPER_HEAT_ERROR_HAPPEN,       //< 刮刀加热发生故障
    PRINT_SCRAPER_HEAT_LOW,                //< 刮刀加热温度过低
    PRINT_HEAT_RESIN_LOW,                  //< 加热警告: 树脂液位过低
    PRINT_SCRAPER_LIMIT_ABNORMAL,          //< 刮刀限位异常(两边都触发了限位,传感器异常)
    PRINT_FORCE_SENSOR_DISCONNECTION,      //< 力值称重传感器连接异常
    PRINT_LOAD_CACHE_TIMEOUT,              //< 投图错误: 投图超时
    PRINT_LOAD_CACHE_ERROR_IMGAE,          //< 投图错误: 例如投了一张黑图

    INFLATOR_UPGRADE_FILE_NOT_EXISTS = 3000, // 充气机升级文件不存在
    INFLATOR_UPGRADE_NOT_CONNECT,            // 充气机未连接
    INFLATOR_UPGRADE_MD5_CHECK_ERROR,        // 充气机MD5校验错误
    INFLATOR_UPGRADE_OFFLINE_ERROR,          // 充气机升级中离线错误
    TEMPER_UPGRADE_FILE_NOT_EXISTS,          // 温控模块升级文件不存在
    TEMPER_UPGRADE_MD5_CHECK_ERROR,          // 温控模块MD5校验错误
    TEMPER_UPGRADE_OFFLINE_ERROR,            // 温控模块升级中离线错误
    FORCE_UPGRADE_FILE_NOT_EXISTS,           // 温控模块升级文件不存在
    FORCE_UPGRADE_MD5_CHECK_ERROR,           // 温控模块MD5校验错误
    FORCE_UPGRADE_OFFLINE_ERROR,             // 温控模块升级中离线错误
    NFC_MASK_READ_FAILED,                    // NFC模块读取失败
    NFC_MASK_CRC_ERROR,                      // NFC模块CRC校验错误
    NFC_MASK_DEVID_ERROR,                    // NFC模块设备ID错误
    NFC_MASK_INVALID_VALUE,                  // NFC屏幕校准文件无效
    NFC_MASK_WHITE,                          // MASK文件全是255

    PRINT_OTHERS,

    //SLAVE_ERROR
    SLAVE_ERROR = 4000,
    SLAVE_ZERO_PRESSURE_OVER,               //< 寻零失败:压力过大
    SLAVE_ZERO_DOWN_PULL_OVER,              //< 向下的拉力过大
    SLAVE_ZERO_TOP_LIMIT,                   //< 寻零失败:意外遇到主轴上限位
    SLAVE_ZERO_BOTTOM_LIMIT,                //< 寻零失败:意外遇到主轴下限位
    SLAVE_ZERO_NO_TOP_LIMIT,                //< 寻零失败:无法找到上限位
    SLAVE_ZERO_NO_BOTTOM_LIMIT,             //< 寻零失败:无法找到下限位
    SLAVE_ZERO_NO_OUT_TOP_LIMIT,            //< 寻零失败:无法走出上限位
    SLAVE_ZERO_DROP_DETECT,                 //< 寻零失败:掉版检测异常
    SLAVE_ZERO_NO_CLEAR_FORCE,              //< 寻零失败:压力传感器没有归零
    SLAVE_ZERO_TIMEOUT,                     //< 寻零失败:寻零超时
    SLAVE_ZERO_FOUND_FAIL,                  //< 寻零失败:找不到寻零
    SLAVE_ZERO_OUT_GUARD_SFIFT,             //< 寻零失败:超出主轴最大运行距离
    SLAVE_ZERO_FORCE_ABNORMAL,              //< 寻零警告:寻零力值异常抖动(力传感器异常)
    SLAVE_ZERO_RESIN_LOW,                   //< 寻零警告:寻零判断液位不足
    INFLATOR_LOWER_DOWN_LIMIT,              //< 脉冲目标压差超出上限位警告 或 流量偏高警告
    INFLATOR_GAS_PATH_ABNORMAL,             //< 脉冲气路异常
    INFLATOR_OVER_TOP_LIMIT,                //< 脉冲目标压差超出上限位警告 或 流量偏高警告

    SLAVE_FRAME_ERROR = 4997,               //< 帧数据错误
    SLAVE_DEVICE_BUSY = 4998,               //< 设备正忙碌
    SLAVE_OTHER_ERROR = 4999,               //< 其他错误

    //SYS_ERROR
    SYS_ERROR = 6000,

    //MODEL_PARSER_ERROR
    MODEL_FILE_TYPE_NOT_SUPPORT = 8000, //格式不支持
    MODEL_PLACEHOLDER_1,
    MODEL_FILE_NOT_FOUND,          //文件不存在
    MODEL_FILE_NOT_OPEN,          //文件打开失败
    MODEL_LAYERS_ERROR,       //模型层数错误
    MODEL_DIR_CLEAN,          //目录清理失败
    MODEL_FIND_TECHBAG,       //未找到工艺包
    MODEL_TEMP_MISS,          //获取临时文件目录失败
    MODEL_CREATE_MODELDIR,    //创建临时目录失败
    MODEL_BUILD_FILE_MISS,    //找不到buildscript.ini
    MODEL_PARAMS_FILE_MISS,   //找不到parameters.ini
    MODEL_LAYER_PARAMS_ERROR, //解析buildscript.ini层数错误
    MODEL_FIND_KEY_ERROR,     //找不到对应的键值配置
    MODEL_REMOVE_TEMP_ERROR,   //删除临时目录失败
    MODEL_PARAMS_AIR_ERROR,   //脉冲模块配置错误
    MODEL_IAMGE_NOT_FOUND,    //找不到图片文件
    MODEL_EXTRACT_RUNNING,    //解压线程运行中
    MODEL_FILE_SIZE_ERROR,    //获取文件解压大小错误
    MODEL_LOW_DISK_SPACE,   //解压空间不足

    TEACHBAG_REMOVE_ALL,          //工艺包临时目录删除失败
    TECHBAG_FILE_COPY,            //工艺文件拷贝失败
    TECHBAG_FILE_EXTRACT,         //工艺包解压失败
    TECHBAG_MODE_TAR_MISS,        //找不到解压文件
    TECHBAG_MD5_READ_ERROR,       //工艺包md5错误
    TECHBAG_MD5_FILE_MISS,        //找不到md5文件
    TECHBAG_MD5_CHECK,            //md5校验失败
    TECHBAG_CONFIG_MISS,          //找不到配置文件
    TECHBAG_BASE_SGY_MISS,        //获取常规工艺包策略失败
    TECHBAG_ADAPTIVE_SGY_MISS,    //获取自适应工艺包策略失败
    TECHBAG_CONTOURFILL_SGY_MISS, //获取轮廓填充工艺包策略失败
    TECHBAG_MOTOR_SGY_MISS,       //获取电机运动参数工艺包策略失败
    TECHBAG_CONTENT_EMPTY,        //工艺包内容为空
    TECHBAG_CONFIG_PARSE_ERROR,   //配置文件解析错误
    TECHBAG_CONFIG_READ_ERROR,    //配置文件读取错误
    TECHBAG_CONFIG_MODE_EMPTY,    //配置模式为空
    TECHBAG_DECRYPT_ERROR,        //解密失败
    TECHBAG_NORMAL_JSON_EMPTY,    //常规工艺包策略为空
    TECHBAG_NORMAL_PARSE_ERROR,   //常规工艺包策略解析错误
    TECHBAG_ADAPTIVE_PARSE_EMPTY, //自适应工艺包策略为空

    //PROJECTOR_ERROR
    PROJECTOR_ERROR = 9000,
};

}

} // namespace heygears::defines

#endif // GLOBAL_DEFINES_H
