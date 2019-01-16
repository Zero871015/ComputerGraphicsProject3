#version 430 core

in vec2 vUV;

uniform sampler2D Texture;

out vec4 fColor;
uniform float shaderType;
uniform float time;

void main()
{
    //正常
    if(shaderType==0)
    {
        fColor = (texture2D(Texture,vUV));
    }
    //暈眩
    else if(shaderType==1)
    {
        fColor = (texture2D(Texture,vUV)+
        texture2D(Texture,vUV+vec2(0.02*sin(time), 0.02*cos(time))))*0.5;
    }
    //邊線
    else if(shaderType==2)
    {
        fColor = texture2D(Texture,vUV);
        vec4 temp =
        (texture2D(Texture,vUV)+
        texture2D(Texture,vUV+vec2(0.002,0))+
        texture2D(Texture,vUV+vec2(-0.002,0))+
        texture2D(Texture,vUV+vec2(0,0.002))+
        texture2D(Texture,vUV+vec2(0,-0.002))
        )*0.2;
        fColor=vec4(1,1,1,1)-vec4(abs(fColor.r-temp.r),abs(fColor.g-temp.g),abs(fColor.b-temp.b),0.0);
        //fColor = temp;
    }
    //負片
    else if(shaderType==3)
    {
        fColor = vec4(1,1,1,1)-texture2D(Texture,vUV);
    }
    //動態模糊
    else if(shaderType==4)
    {
        float k = sqrt((0.5-vUV.x)*(0.5-vUV.x)+(0.5-vUV.y)*(0.5-vUV.y));
        fColor = texture2D(Texture,vUV);
        vec4 temp = 
        (texture2D(Texture,vUV)+
        texture2D(Texture,vUV+vec2(0.02,0))+
        texture2D(Texture,vUV+vec2(-0.02,0))+
        texture2D(Texture,vUV+vec2(0,0.02))+
        texture2D(Texture,vUV+vec2(0,-0.02))
        )*0.2;
        fColor = (1-k)*fColor+k*temp;
    }
    
}
