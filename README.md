# underpan_v3.1
    WTR21级电控组国庆任务；
    
### 更新说明
   - v_3.0  基本实现基础运动功能
   - v_3.1  增加了四轮全向移动底盘功能
  
### 1.项目说明

#### 机械部分
###### 1.使用材料
- 6寸软橡胶全向轮  X  3
- 50X50碳板、铝管若干
- M5*30螺丝 X 32
- M5螺母 X 32
- DJI电机3508、电调c620等套件 X 3
- SYM32F407c8t6开发板
- DJI电源TB47D
###### 2.底盘加工
- 铝管切割，钻孔
- 碳板切割
- 组装
###### 3.文件内容
- 底盘作图的dwg文件（保留尺寸数据）
- 底盘dxf文件（激光切割）
- .NC文件（切割碳板）

#### 代码部分
- usercode文件夹，可轻松移植
- 在usermain.c和usermain.h中根据需求修改代码即可
