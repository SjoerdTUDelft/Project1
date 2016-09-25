#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class WorldObject {
public:
	glm::mat4x4 transform;
	WorldObject(float x =0, float y=0, float z=0) : 
		transform(0.0,0.0,0.0,x,
				  0.0,0.0,0.0,y,
				  0.0,0.0,0.0,z,
				  0.0,0.0,0.0,1.0)
	
	{
			
	}


};