# dx_test
利用direct3d实现3d场景载入，人体骨骼动画，智能摄像头，阳光，蝴蝶，花草。    采用最新vs2017编译。

![](https://github.com/czl0325/dx_test/blob/master/demo.gif?raw=true)

## 打开工程遇到的错误：

1. error MSB8036: 找不到 Windows SDK 版本10.0.17134.0。请安装所需的版本的 Windows SDK 或者在项目属性页中或通过右键单击解决方案并选择“重定解决方案目标”来更改 SDK 版本。

请在配置属性，常规，把windows sdk版本改成你电脑有的

2. 错误	C1083	无法打开包括文件: “d3dx9.h”: No such file or directory	

首先要下载directx，下载地址 https://www.microsoft.com/en-us/download/details.aspx?id=6812
安装过程中如果遇到.NET framework 3.5无法安装，请 右键我的电脑->管理->服务，找到windows update，双击后，把启动类型中禁用改成手动。
安装成功后，在vs2017中，右键项目->配置属性->vc++目录
包含目录中添加C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include
库目录中添加C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86
