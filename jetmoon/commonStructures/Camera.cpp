#include "Camera.hpp"

#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>  
#include <GL/glew.h>


void Camera::updateViewTransform(){
	viewTransform = glm::mat4(1.0f);
	viewTransform = glm::scale(viewTransform, glm::vec3(scale * ratioX, scale, 1/9999.));
	viewTransform = glm::rotate(viewTransform, rotation, glm::vec3(0.f, 0.f, 1.0f));
	viewTransform = glm::translate(viewTransform, glm::vec3(-position.x, -position.y, 0.f));
}

glm::mat4 Camera::getTransformationMatrix(){
	return viewTransform;
}

Vec3 Camera::worldSpaceToScreenSpace(const Vec3& origin){
	auto glmVec = glm::vec3(viewTransform * glm::vec4(origin.x, origin.y, origin.z, 1.));
	return Vec3{glmVec.x, glmVec.y, glmVec.z};
}

Vec3 Camera::worldSpaceToScreenSpace(const glm::mat4& tr){
	auto glmVec = glm::vec3(viewTransform * tr * glm::vec4(0., 0., 0., 1.));
	return Vec3{glmVec.x, glmVec.y, glmVec.z};
}


Vec2 Camera::worldSpaceToScreenPixelSpace(const Vec3& origin){
	auto vec = worldSpaceToScreenSpace(origin);
	vec = (vec + 1) / 2.;
	GLint viewport[4]{};
	glGetIntegerv(GL_VIEWPORT, viewport);
	return Vec2{vec.x*viewport[2], viewport[3] - vec.y*viewport[3]};
}

Vec2 Camera::worldSpaceToScreenPixelSpace(const glm::mat4& tr){
	auto vec = worldSpaceToScreenSpace(tr);
	vec = (vec + 1) / 2.;
	GLint viewport[4]{};
	glGetIntegerv(GL_VIEWPORT, viewport);
	return Vec2{vec.x*viewport[2], viewport[3] - vec.y*viewport[3]};
}
