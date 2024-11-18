주요 기능
OpenGL 초기화:
glAllInit() 함수는 OpenGL 환경을 초기화하고, GLFW와 GLEW를 설정합니다.
창 크기를 1920x1080으로 지정하고, OpenGL 3.3 코어 프로파일을 사용합니다.
쉐이더 초기화:
프로젝트에서 다양한 쉐이더(Shader)를 사용합니다. 여기에는 텍스트 렌더링, 버튼 조명 효과, 기본 조명 효과 등이 포함됩니다.
쉐이더는 각 객체의 외관과 조명 효과를 담당합니다.
텍스처 로드:
loadTexture() 함수는 BMP 파일(source.bmp)을 텍스처로 로드하고 이를 OpenGL 객체에 바인딩합니다.
텍스처는 배경 또는 특정 객체의 표면에 적용됩니다.
카메라와 뷰 설정:
view 행렬은 카메라의 위치(cameraPos)와 관찰 방향(cameraAt)을 기반으로 생성됩니다.
이 행렬은 OpenGL의 glm::lookAt를 통해 설정됩니다.
애니메이션 처리:
updateAnimData() 함수는 키프레임 데이터를 기반으로 큐브들의 애니메이션 상태를 업데이트합니다.
각 큐브는 시간에 따라 위치와 회전을 변경하며, 키프레임은 KeyFraming 클래스를 사용해 관리됩니다.
점수 시스템:
사용자 입력에 따라 점수를 계산합니다. Q, W, E, R 키에 따라 맞춤 정확도가 다릅니다:
Perfect: +5점
Good: +3점
Hit: +1점
Miss: -2점
점수는 화면에 실시간으로 렌더링됩니다.
텍스트 렌더링:
Text 클래스를 사용하여 텍스트를 화면에 표시합니다.
게임 상태(예: "Press ENTER to play game") 및 점수는 실시간으로 업데이트됩니다.
객체 렌더링:
각 큐브와 버튼은 OpenGL을 통해 렌더링됩니다.
glm::translate와 glm::scale을 사용해 각 객체의 위치와 크기를 설정합니다.
쉐이더와 함께 조명 효과를 적용하여 객체의 시각적 품질을 개선합니다.
