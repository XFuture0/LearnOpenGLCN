#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image/stb.image.h"
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
// 顶点着色器
/*#version 330 core//版本号 + 是否使用核心模式
layout(location = 0) in vec3 aPos;//在顶点着色器中声明所有的输入顶点属性,现在只有一个三维坐标

void main()
{
//为了设置顶点着色器的输出，我们必须把位置数据赋值给预定义的gl_Position变量
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);//vec.w分量不是用作表达空间中的位置
}*/
//片段着色器
/*#version 330 core
out vec4 FragColor;//它表示的是最终的输出颜色，我们应该自己将其计算出来。声明输出变量可以使用out关键字

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);//给颜色赋值输出
}*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) 
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//返回ESC是否正在被按下
    {
        glfwSetWindowShouldClose(window, true);//告诉GLFW关闭窗口
    }
}
int main(void)
{
    glm::mat4 trans = glm::mat4(1.0f);//制造一个旋转矩阵
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));//沿z轴旋转90度
    trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));//缩放0.5倍
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 1.0f, 0.0f));//模型矩阵：沿x轴旋转55度
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));//视图矩阵：物体向后平移3.0f个单位，相当于摄像机向前移动3.0f个单位
    projection = glm::perspective(glm::radians(45.0f),800.0f/600.0f , 0.1f, 100.0f);//投影矩阵：宽高比是视口的宽高比
    //glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);//摄像机方向向量(标准化)，局部z轴
    //glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));//摄像机右向量(标准化)，局部x轴，将局部z轴和向上轴叉乘，得到x轴
    //glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);//摄像机向上向量(标准化)，局部y轴
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);//摄像机位置
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);//摄像机向前向量
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);//摄像机向上向量
    glm::vec3 lightPos(1.5f, 0.5f, -0.5f);
    glm::mat4 lightmodel = glm::mat4(1.0f);
    lightmodel = glm::translate(lightmodel, lightPos);
    lightmodel = glm::scale(lightmodel, glm::vec3(0.5f));
    //创建观察矩阵
    view = glm::lookAt(cameraPos,cameraPos + cameraFront,cameraUp);//第二个参数是摄像机目标位置
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"//记得设置位置值
        "layout (location = 1) in vec3 aColor;\n"//设置颜色属性
        "layout (location = 2) in vec2 aTexCoord;\n"//设置纹理坐标属性
        "layout (location = 3) in vec3 aNormal;\n"//设置法线属性
        "out vec3 VertexColor;\n"//当类型和名字都一样的时候，OpenGL就会把两个变量链接到一起
        "out vec2 TexCoords;\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   VertexColor = aColor;\n"
        "   TexCoords = aTexCoord;\n"
        "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"//法线矩阵被定义为「模型矩阵左上角3x3部分的逆矩阵的转置矩阵」,获得世界空间的法线向量
        "   FragPos = vec3(model * vec4(aPos,1.0f));\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "in vec3 VertexColor;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform vec4 ourColor;\n"//uniform是全局变量，可以被着色器程序的任意着色器在任意阶段访问
        "uniform sampler2D texture1;\n"//纹理采样器，一个纹理的默认纹理单元是0，它是默认的激活纹理单元
        "uniform sampler2D texture2;\n"
        "void main()\n"
        "{\n"
        "   FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2);\n"//GLSL内建的texture函数来采样纹理的颜色,第一个参数是纹理采样器，第二个参数是对应的纹理坐标
        "}\0";
    const char* ObjShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 Normal ;\n"//获取法线
        "in vec3 FragPos ;\n"//获取世界坐标
        "in vec2 TexCoords;\n"
        "uniform vec3 lightColor;\n"
        "uniform vec3 objectColor;\n"
        "uniform vec3 lightPos;\n"//光源位置
        "uniform vec3 viewPos;\n"//摄像机位置
        "struct Material {\n"//材质实际上就是环境光，漫反射，高光上颜色的组合
        "   vec3 ambient;\n"//环境光材质(在纹理上环境光颜色在几乎所有情况下都等于漫反射颜色，可以舍去)
        "   sampler2D diffuse;\n"//漫反射材质,表现了物体所有的漫反射颜色的纹理图像
        "   sampler2D specular;\n"//高光材质,镜面高光的强度可以通过图像每个像素的亮度来获取
        "   float shininess;\n"//反光度
        "};\n"
        "uniform Material material;\n"//材质结构体
        "struct Light {\n"//材质实际上就是环境光，漫反射，高光上颜色的组合
        "   vec3 ambient;\n"//环境光权值
        "   vec3 diffuse;\n"//漫反射权值
        "   vec3 specular;\n"//高光权值
        //"   vec3 position;\n"//反光度权值（光源的位置）
        "   vec3 dirction;\n"//平行光方向，这里使用的是由光源发出的平行光
        "   float constant;\n"//光线衰减系数常数
        "   float linear;\n"//光线衰减系数一次项
        "   float quadratic;\n"//光线衰减系数二次项
        "   vec3 direction;\n"//聚光的方向向量
        "   float cutoff;\n"//内切光角
        "   float outercutoff;\n"//外切光角
        "};\n"
        "uniform Light light;\n"//材质结构体
        "void main()\n"
        "{\n"
        "   vec3 norm = normalize(Normal);\n"//法线标准化
        "   vec3 lightDir = normalize(-light.dirction);\n"//光线向量标准化
        "   vec3 LightDir = normalize(viewPos - FragPos);\n"//聚光光线向量标准化
        "   vec3 viewDir = normalize(viewPos - FragPos);\n"//观察者向量标准化
        "   float distance = length(lightPos - FragPos);\n"//获取片段位置到光源位置的距离
        "   float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n"//光线衰减公式
        "   float theta = dot(LightDir,normalize(-light.direction));\n"//获取片段位置到聚光光源位置的夹角点乘
        "   float intensity =clamp((theta - light.outercutoff) / (light.cutoff - light.outercutoff),0.0f,1.0f);\n"//聚光强度衰减公式
        "   vec3 reflectDir = reflect(-lightDir,norm);\n"//获取观察者向量对于法线的反射向量,前面取反是因为函数第一个向量是从观察者指向片段位置的向量，而我们先前计算的是片段指向观察者
        //32是高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强
        "   float spec = pow(max(dot(viewDir,reflectDir),0.0f),material.shininess);\n"//计算视线方向与反射方向的点乘（并确保它不是负值），然后取它的32次幂(反光度)
        "   float diff = max(dot(norm,lightDir),0.0f);\n"//法线与光线向量点乘获得光照系数
        "   vec3 diffuse = intensity * attenuation * light.diffuse * (vec3(texture(material.diffuse,TexCoords)) * diff) * lightColor;\n"//漫反射对颜色的贡献
        "   vec3 ambient = intensity * attenuation * light.ambient * vec3(texture(material.diffuse, TexCoords)) * lightColor;\n"//全局光照对颜色的贡献
        "   vec3 specular = attenuation * light.specular * vec3(texture(material.specular,TexCoords)) * spec * lightColor;\n"//高光对颜色的贡献
        "   FragColor = vec4((diffuse + ambient + specular),1.0);\n"
        "}\0";
    const char* LightShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f,1.0f,1.0f,1.0f);\n"
        "}\0";
    //由于OpenGL是在3D空间中工作的，而我们渲染的是一个2D三角形，我们将它顶点的z坐标设置为0.0。这样子的话三角形每一点的深度(Depth，译注2)都是一样的，从而使它看上去像是2D的。
    //OpenGL中我们指定的所有坐标都是3D坐标（x、y和z）,若只有两个float值默认z=0
    float vertices[] = {
        // 位置              // 颜色           //  纹理坐标
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,0.0f,  // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,0.0f,  // 左下
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f,1.0f,  // 右上
         -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,1.0f   // 左上
    };//三角形的三个顶点数据
    float vertices3D[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3
    };//索引数据
    if (!glfwInit())//初始化GLFW
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);//主版本3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);//次版本3->3.3，告诉GLFW我们要使用的OpenGL版本是3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);//告诉GLFW我们使用的是核心模式(Core-profile)
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);//创建一个Window窗口,前两个参数控制窗口的大小
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK)//初始化GLEW，位置要在创建窗口之后，GLEW明确要在有明确的上下文之后才能初始化
    {
        std::cout << "Error" << std::endl;
    }
    glEnable(GL_DEPTH_TEST);//开启深度测试
    //glDepthMask(GL_FALSE);//禁用深度测试写入（仅可读）
    glDepthFunc(GL_LESS);//设置深度测试函数,默认小于
    glEnable(GL_STENCIL_TEST);//启用模板测试
    glStencilMask(0x00); // 禁止模板缓冲的写入
    glStencilMask(0xFF); // 启用模板缓冲写入
    glStencilMask(0xFF);//设置模板缓冲区写入掩码(对写入模板缓冲的数据而言)
    glStencilFunc(GL_ALWAYS,1,0xFF);//设置模板测试函数,仅仅描述了OpenGL应该对模板缓冲内容做什么,第三个参数会与储存的模板值(上一行)在测试比较它们之前进行与(AND)运算
    //模板测试失败时采取的行为(1),模板测试通过，但深度测试失败时采取的行为(2),模板测试和深度测试都通过时采取的行为(3)
    glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);//设置模板测试操作,默认为三个Keep,保持当前储存的模板值
    glEnable(GL_BLEND);//启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//设置混合函数,源因子alpha(前)，目标因子alpha(后)
    glBlendEquation(GL_FUNC_ADD);//设置混合方程式,默认是相加
    //默认情况下，逆时针顶点所定义的三角形将会被处理为正向三角形，顺时针顶点所定义的三角形将会被处理为反向三角形
    //glEnable(GL_CULL_FACE);//启用面剔除
    //glCullFace(GL_FRONT);//设置剔除模式为正向面,glCullFace的初始值是GL_BACK(剔除反向面)
    //glFrontFace(GL_CCW);//设置顶点顺序为逆时针，默认是逆时针，GL_CW代表的是顺时针顺序。
    unsigned int texture1;
    glGenTextures(1,&texture1);//生成材质
    glBindTexture(GL_TEXTURE_2D,texture1);//绑定材质，以下参数设置会记录在这个材质里面
    //设置纹理参数
    //多级渐远纹理仅使用在纹理被缩小的情况下
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//纹理目标，纹理坐标S轴，环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//纹理目标，纹理坐标T轴，环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//纹理目标，缩小时,过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理目标，放大时，过滤方式
    //加载纹理
    int width, height, nrChannels;//宽，高，颜色通道数
    unsigned char* data = stbi_load("container2.png", &width, &height, &nrChannels, 0);//获取图片数据
    //载入纹理数据
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);//纹理目标，多级渐远纹理级别，纹理储存格式，纹理宽度，纹理高度，0(历史遗留问题),原图的格式，储存为字节数组，图像数据
    glGenerateMipmap(GL_TEXTURE_2D);//生成多级渐远纹理
    stbi_image_free(data);//释放图片数据
    unsigned int texture2;
    glGenTextures(1, &texture2);//生成材质
    glBindTexture(GL_TEXTURE_2D, texture2);//绑定材质
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//纹理目标，纹理坐标S轴，环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//纹理目标，纹理坐标T轴，环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//纹理目标，缩小时,过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理目标，放大时，过滤方式
    stbi_set_flip_vertically_on_load(true);//翻转图片的纹理坐标的y轴
    data = stbi_load("container2_specular.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//png图片要额外考虑alpha通道
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    //前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）
    //OpenGL坐标范围只为-1到1,以视口中心为中心点
    //通过使用由glViewport函数提供的数据，顶点坐标在顶点着色器处理成标准化设备坐标,进行视口变换，标准化设备坐标会变换为屏幕空间坐标
    glViewport(0, 0, 800, 600);//OpenGL渲染窗口的尺寸大小(视口)
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);//创建一个顶点数组对象
    glBindVertexArray(VAO);//绑定顶点数组对象,任何随后的顶点属性调用都会储存在这个VAO中,用于存储后面的渲染状态
    unsigned int VBO;
    glGenBuffers(1,&VBO);//生成一个带有缓冲ID的VBO对象
    //OpenGL有很多缓冲对象类型，顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER
    //从这一刻起，我们使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);//把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上，即确定他为顶点缓冲对象
    //它的第一个参数是目标缓冲的类型。第二个参数指定传输数据的大小(以字节为单位)，用一个简单的sizeof计算出顶点数据大小就行。第三个参数是我们希望发送的实际数据。
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3D), vertices3D, GL_STATIC_DRAW);//把用户定义的数据复制到当前绑定缓冲的函数
    unsigned int EBO;
    glGenBuffers(1, &EBO);//创建一个索引缓冲对象EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//绑定索引缓冲对象EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //使用数据的类型
    //GL_STATIC_DRAW ：数据不会或几乎不会改变。
    //GL_DYNAMIC_DRAW：数据会被改变很多。
    //GL_STREAM_DRAW ：数据每次绘制时都会改变。
    //非标准化是-1到1，标准化是0到1
    //设定顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);//顶点位置值，顶点向量大小，数据类型，是否标准化，步长，起始位置偏移量
    glEnableVertexAttribArray(0);//启用顶点属性数组,里面填位置值
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));//偏移量起始值是在第四个float值
    //glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(3);
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);//创建顶点着色器
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);//把这个着色器源码附加到着色器对象,第二参数指定了传递的源码字符串数量
    glCompileShader(vertexShader);//编译顶点着色器,在附加后要编译
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//创建片段着色器
    glShaderSource(fragmentShader,1,&ObjShaderSource,NULL);
    glCompileShader(fragmentShader);
    int success;
    char infolog[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);//检查顶点着色器的编译状态
    if (!success) 
    {
        glGetShaderInfoLog(vertexShader,512,NULL,infolog);//获得错误消息
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infolog << std::endl;
    }
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();//创建一个着色器程序对象
    glAttachShader(shaderProgram, vertexShader);//着色器附加到着色器程序
    glAttachShader(shaderProgram, fragmentShader);//着色器附加到着色器程序
    glLinkProgram(shaderProgram);//链接以上着色器到着色器程序上
    glUseProgram(shaderProgram);//使用着色器程序对象,绑定的顶点缓冲是最近一次的glBindBuffer
    //在把着色器对象链接到程序对象以后，记得删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    unsigned int fragmentShader_Light;
    fragmentShader_Light = glCreateShader(GL_FRAGMENT_SHADER);//创建片段着色器
    glShaderSource(fragmentShader_Light, 1, &LightShaderSource, NULL);
    glCompileShader(fragmentShader_Light);
    unsigned int LightShaderProgram;
    LightShaderProgram = glCreateProgram();
    glAttachShader(LightShaderProgram,vertexShader);
    glAttachShader(LightShaderProgram, fragmentShader_Light);
    glLinkProgram(LightShaderProgram);
    glUseProgram(LightShaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader_Light);
    //当窗口被第一次显示的时候framebuffer_size_callback也会被调用
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//告诉GLFW我们希望每当窗口调整大小的时候调用这个函数
    //希望程序在我们主动关闭它之前不断绘制图像并能够接受用户输入
    //glfwWindowShouldClose函数在我们每次循环的开始前检查一次GLFW是否被要求退出
    glfwSwapInterval(1);//设置帧速率
    float deltaTime = 0.0f; // 当前帧与上一帧的时间差
    float lastFrame = 0.0f; // 上一帧的时间
    while (!glfwWindowShouldClose(window))//渲染循环(Render Loop)
    {
        glBindVertexArray(VAO);//绑定顶点数组对象
        float cameraSpeed = 2.5f * deltaTime;
        processInput(window);//检查是否按下了ESC
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUseProgram(shaderProgram);//使用着色器程序对象
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//清空颜色缓冲,状态设置函数
        glClear(GL_COLOR_BUFFER_BIT);//清空颜色缓冲,状态使用函数,使用了当前的状态来获取应该清除为的颜色
        glClear(GL_DEPTH_BUFFER_BIT);//清除深度缓冲
        glClear(GL_STENCIL_BUFFER_BIT);//清空模板缓冲
        //主要渲染方式
        //渲染指令
        //float timeValue = glfwGetTime();//获取当前时间
        //float greenValue = (sin(timeValue)/2.0f)+0.5f;
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");//获取uniform变量ourColor的存储位置,-1就代表没有找到这个位置值
        //更新一个uniform之前你必须先使用程序（调用glUseProgram)
        //glUniform4f(vertexColorLocation,0.0f,greenValue,0.0f,1.0f);
        //glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 1.0f, 0.5f, 0.31f);
        //glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 64.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.dirction"), -0.2f, -1.0f, -0.3f);
        glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), 0.032f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.direction"), cameraFront.x, cameraFront.y, cameraFront.z);
        glUniform1f(glGetUniformLocation(shaderProgram, "light.cutoff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(shaderProgram, "light.outercutoff"), glm::cos(glm::radians(17.5f)));
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPos.x,cameraPos.y,cameraPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f,0.5f,0.31f);
        //glUniform1i(glGetUniformLocation(shaderProgram,"texture1"),0);//分配纹理单元
        //glUniform1i(glGetUniformLocation(shaderProgram,"texture2"),1);
        //model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.5f, 1.0f, 0.0f));//第二个参数是旋转增量
        //glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"transform"),1,GL_FALSE,glm::value_ptr(trans));//赋值参数，矩阵数量，是否转置，矩阵数据
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,GL_FALSE,glm::value_ptr(projection));
        glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"),0);
        glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"),1);
        glActiveTexture(GL_TEXTURE0);//激活0号纹理单元，即默认纹理单元
        glBindTexture(GL_TEXTURE_2D, texture1);//给0号纹理单元绑定纹理
        glActiveTexture(GL_TEXTURE1);//激活1号纹理单元
        glBindTexture(GL_TEXTURE_2D, texture2);//给1号纹理单元绑定纹理
        //glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);//绘制图元类型,索引个数，类型，偏移量
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);//解绑顶点数组对象
        glBindVertexArray(lightVAO);
        glUseProgram(LightShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(LightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lightmodel));
        glUniformMatrix4fv(glGetUniformLocation(LightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(LightShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //确保你没有在解绑VAO之前解绑索引数组缓冲，否则它就没有这个EBO配置
        glBindVertexArray(0);//解绑顶点数组对象
        //检查并调用事件，交换缓冲
        //它是一个储存着GLFW窗口每一个像素颜色值的大缓冲，它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
        //我们应用双缓冲渲染窗口应用程序。前缓冲保存着当前最终输出的图像，它会在屏幕上显示；而所有的的渲染指令都会在后缓冲上绘制。当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来，之前提到的不真实感就消除了。
        glfwSwapBuffers(window);//交换颜色缓冲
        glfwPollEvents();//检查有没有触发什么事件
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
    glfwTerminate();//释放/删除之前的分配的所有资源
    return 0;
}