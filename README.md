# 商场安全管理系统
***嵌入式大作业-基于FreeRTOS的多任务系统***

**项目运行环境**
- **软件环境**
基于 _Flask_ 框架的交互界面

- **仿真环境**
基于 _Proteus_ 的嵌入式仿真系统，芯片选择 _Arduino328_ 

**项目运行教学**

1. ___在Proteus中，芯片载入二进制文件 _.elf_ 或 _.hex_ ，启动运行仿真
2. 在 runserver.py 子目录下，终端运行___
    ```python
    python runserver.py
    ```

**项目文件结构**
* Arduino 328
    > Arduino 328.pdsprj
* Binary File
    > src.ino.dlf
    > src.ino.hex
* FlaskWebProject1
    * FlaskWebProject1
        > runserver.py
    * FlaskWebProject1.sln
* src
    > src.ino
* requirements.txt


