#include "components/Transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/matrix_decompose.hpp>


glm::mat4 toMatrix(Transform& tr){
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(tr.position.x, tr.position.y, tr.position.z));
	float r = tr.rotation;
	trans = glm::rotate(trans, r, glm::vec3(0.f, 0.f, 1.0f));
	trans = glm::scale(trans, glm::vec3(tr.scale.x, tr.scale.y, 1.f));
	return trans;
}

Transform fromMatrix(glm::mat4 transformation){
	Transform tr;
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(transformation, scale, rotation, translation, skew,perspective);

	auto rotation2 = glm::roll(rotation);

	tr.position.x = translation[0];
	tr.position.y = translation[1];
	tr.position.z = translation[2];
	tr.scale.x = scale[0];
	tr.scale.y = scale[1];
	tr.rotation = rotation2;
	return tr;
}
