#version 400

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform vec3 triangleColor;

in vec3 position;
in vec3 color;
in vec2 texCoord;
out vec4 frag_colour;


vec2 v[10]; vec2 vt[10]; bool tri[16]; vec2 disp[6];
const float phi = (1.0+sqrt(5.0))/2.0;
const float pi = 3.14159265359;

#define AA
const int iter = 4 ;

vec2 makePoint(float offset) {
       return vec2( cos(pi*2.0*offset),sin(pi*2.0*offset));
}

//http://stackoverflow.com/questions/13300904/determine-whether-point-lies-inside-triangle
bool triangle(vec2 p, vec2 p1, vec2 p2, vec2 p3,vec2 d) {
    p1+=d;p2+=d;p3+=d;
    float alpha = ((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) /
        ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
    float beta = ((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) /
           ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
    float gamma = 1.0 - alpha - beta;
    return (alpha > 0.0 && beta > 0.0 && gamma > 0.0);
}


void main()
{
	vec2 UV = texCoord;
	float rescale = 0.6;
    UV*=rescale;
    //Scaling outer and inner vectors star
    float outer = 0.2;
    float inner = outer/(phi+1.0);
	
    for(int i = 0; i < 5; i ++) {
        //Outervectors
    	v[i] = outer*makePoint(0.05+float(i)*0.2);
        //Innervectors
        v[i+5] = inner*makePoint(0.15+float(i)*0.2);
    }
    
    //Vectors bottomrow
    float scale = 3.0/20.0;
    for(int i = 0; i < 10; i++) {
    	vt[i] = scale*v[i];   
    }
    
    //Displacement bottomrow
    float starwidth = (cos(0.05*2.0*pi))*2.5*outer*scale;
    for(int i = 0; i < 6; i++) {
        disp[i].y = 0.11;
    	disp[i].x =  0.5*rescale-starwidth*2.5 + starwidth*float(i);   
    }
    float close = 0.0066;
    disp[2].x += close;
    disp[3].x -= close;
    //Displace Blackstar
    vec2 displace = vec2(0.5*rescale,0.35 );
    vec3 col;
    
    #ifdef AA
    vec2 dx = dFdx(UV)/float(iter);
    vec2 dy = dFdy(UV)/float(iter);
    
    //SuperSample
    for(int itx = 0; itx < iter; itx++) {
        for(int ity = 0; ity < iter; ity++) {
            #endif
            vec2 uv = UV;
            #ifdef AA
            uv.x += float(itx)*dx.x - dx.x/2.0;
            uv.y += float(ity)*dy.y - dy.y/2.0;
            #endif
			
            //Blackstar
            tri[0] = triangle(uv,v[0],v[3],v[6],displace);
            tri[1] = triangle(uv,v[1],v[4],v[7],displace);
            tri[2] = triangle(uv,v[4],v[2],v[5],displace);

            //BL Triangle
            tri[3] = triangle(uv,vt[1], vt[4], vt[7],disp[0]);

            //Small Star
            tri[4] = triangle(uv,vt[0],vt[3],vt[6],disp[1]);
            tri[5] = triangle(uv,vt[1],vt[4],vt[7],disp[1]);
            tri[6] = triangle(uv,vt[4],vt[2],vt[5],disp[1]);

            //Arrow right
            tri[7] = triangle(uv,vt[0],vt[2],vt[8],disp[2]);
            tri[8] = triangle(uv,vt[0],vt[3],vt[6],disp[2]);

            //Arrow left
            tri[9] = triangle(uv,vt[0],vt[2],vt[8],disp[3]);
            tri[10] = triangle(uv,vt[2],vt[4],vt[5],disp[3]);

            //Arrow up
            tri[11] = triangle(uv,vt[1],vt[3],vt[9],disp[4]);
            tri[12] = triangle(uv,vt[1],vt[4],vt[7],disp[4]);

            //Broken Star
            tri[13] = triangle(uv,vt[1],vt[4],vt[7],disp[5]);
            tri[14] = triangle(uv,vt[0],vt[6],vt[3],disp[5]);
            bool cull = triangle(uv,vt[3],vt[4],vt[7],disp[5]);

            bool hit = false;
            for(int i = 0; i < 16; i ++) {
                if(tri[i]) {
                    hit = true; 
                    break;   
                }
            }

            if(hit && !cull ) {
                 col += vec3(0.12);
            } else {
                 col += vec3(1.0);                           
            }
            #ifdef AA
        }
    }
    col /=float(iter*iter);
    #endif
    frag_colour = vec4(col,1.0);
}