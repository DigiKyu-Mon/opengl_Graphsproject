/*
Forward 단계: End Effector를 목표 지점으로 이동시키면서 각 관절의 위치를 업데이트합니다. 이 과정에서 체인의 다른 관절들이 당겨져 위치가 변합니다.
Backward 단계: Base (예: 어깨)의 위치를 고정하고 End Effector가 목표에 가까워질 때까지 관절을 다시 조정합니다.
*/

void FABRIK_Solve(std::vector<glm::vec3>& joints, glm::vec3 target, float tolerance) {
    int maxIterations = 10;
    float distanceToTarget = glm::distance(joints.back(), target);

    for (int iteration = 0; iteration < maxIterations && distanceToTarget > tolerance; ++iteration) {
        // 1. Forward Reaching 단계
        joints.back() = target;
        for (int i = joints.size() - 2; i >= 0; --i) {
            float dist = glm::distance(joints[i], joints[i + 1]);
            glm::vec3 direction = glm::normalize(joints[i] - joints[i + 1]);
            joints[i] = joints[i + 1] + direction * dist;
        }

        // 2. Backward Reaching 단계
        glm::vec3 basePosition = joints[0];
        joints[0] = basePosition;
        for (int i = 0; i < joints.size() - 1; ++i) {
            float dist = glm::distance(joints[i], joints[i + 1]);
            glm::vec3 direction = glm::normalize(joints[i + 1] - joints[i]);
            joints[i + 1] = joints[i] + direction * dist;
        }

        // 목표 지점까지의 거리 업데이트
        distanceToTarget = glm::distance(joints.back(), target);
    }
}
