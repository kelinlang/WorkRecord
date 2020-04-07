## OpenGl基础

- 图形渲染管线
  - 第一部分把你的3D坐标转换为2D坐标
  - 第二部分是把2D坐标转变为实际的有颜色的像素
  - 过程
    - 顶点数据
    - 顶点着色器
      - 把3D坐标转为另一种3D坐标
      - 对顶点属性进行一些基本处理
    - 形状(图元)装配
      - 顶点着色器输出的所有顶点作为输入，并所有的点装配成指定图元的形状
    - 几何着色器
      - 把图元形式的一系列顶点的集合作为输入，它可以通过产生新顶点构造出新的（或是其它的）图元来生成其他形状
    - 光栅化
      - 图元映射为最终屏幕上相应的像素，生成供片段着色器(Fragment Shader)使用的片段
    - 片段着色器
      - 计算一个像素的最终颜色
    - 测试与混合
- **标准化设备坐标(Normalized Device Coordinates, NDC)**
  - 顶点坐标已经在顶点着色器中处理过，是一个x、y和z值在**-1.0到1.0**的一小段空间
- 顶点缓冲对象(Vertex Buffer Objects, **VBO**)
  - 在GPU内存(通常被称为显存)中储存大量顶点
- 顶点数组对象(Vertex Array Object, **VAO**)
- 索引缓冲对象(Element Buffer Object，**EBO**，也叫Index Buffer Object，**IBO**)

- 着色器

  - 图形渲染管线中快速处理你的数据

- **OpenGL着色器语言(OpenGL Shading Language, GLSL)**

  - in

    - 输入顶点属性(Input Vertex Attribute)
    - 

  - out

    当类型和名字都一样的时候，OpenGL就会把两个变量链接到一起

  - Uniform ：一种从CPU中的应用向GPU中的着色器发送数据的方式

    - 特性：全局的(Global)
      - 在每个着色器程序对象中都是独一无二的，而且它可以被着色器程序的任意着色器在任意阶段访问
      - 无论你把uniform值设置成什么，uniform会一直保存它们的数据，直到它们被重置或更新。

  - 容器

    - 向量(Vector)：包含有1、2、3或者4个分量的容器
      - 
    - 矩阵(Matrix)

- 顶点着色器(Vertex Shader)

- 变量

  - gl_Position：幕后是`vec4`类型

- **纹理**

  - **纹理坐标(Texture Coordinate)**：用来标明该从纹理图像的哪个部分采样（译注：采集片段颜色）。之后在图形的其它片段上进行片段插值(Fragment Interpolation)。
  - 纹理坐标在x和y轴上，范围为**0到1**之间（注意我们使用的是2D纹理图像）
  - 使用纹理坐标获取纹理颜色叫做**采样(Sampling)**

- 坐标系统

  - 局部空间(Local Space，或者称为物体空间(Object Space))
  - 世界空间(World Space)
  - 观察空间(View Space，或者称为视觉空间(Eye Space))
  - 裁剪空间(Clip Space)
  - 屏幕空间(Screen Space)

- 转换矩阵

  - **模型(Model)**
  - **视图(View)**
  - **投影(Projection)**

- 转换过程

  首先，顶点坐标开始于**局部空间(Local Space)**，称为**局部坐标(Local Coordinate)**，然后经过**世界坐标(World Coordinate)**，**观察坐标(View Coordinate)**，**裁剪坐标(Clip Coordinate)**，并最后以**屏幕坐标(Screen Coordinate)**结束

  ![coordinate_systems](http://learnopengl.com/img/getting-started/coordinate_systems.png)

- 摄像机
  - 摄像机位置简单来说就是世界空间中代表摄像机位置的向量

