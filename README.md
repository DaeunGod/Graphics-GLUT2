# Graphics-GLUT2

> 카메라위치에 axes추가


> (b) cameras
	> (i). 마우스 드래그를 앞뒤로 하면 naxis축을 기준으로 움직인다.
	> (i). 쉬프트 키를 누르고 마우스를 드래그 하면 커서 움직임을 따라 uaxis, vaxis를 따라 움직인다.

	> (ii). 마우스 드래그를 좌우로 하면 처음엔 vaxis방향기준으로 회전하고 q를 누르면 uaxis방향, 한번 더 두르면 naxis방향을 기준으로회전한다.

	> (iii). 마우스 휠의 움직임에 따라 확대 또는 축소 된다.

	> 카메라는 axis를 표시하고 있고 투영방식에 따라 청록색, 노란색 박스가 그려져 있다.
	> (iv). 정적인 CCTV는 청록색 박스가 깜빡거리는 카메라들이다. 주 뷰포트 우측에 각 카메라의 뷰가 표시된다.('p'키를 통해 정적인 CCTV와 정면도-측면도-상면도 표시를 바꿀 수 있다.)

	> (v). '1', '2'키를 이용해 주 카메라와 동적 CCTV를 선택할 수 있다. 동적 CCTV는 마우스 드래그를 통해 원하는 방향을 볼 수 있다. 'q'를 누르면 uaxis회전 한다. 'i', 'k'키를 이용해 동적 CCTV의 far_clip을 조절할 수 있다. 'j', 'l'키를 이용해 near_clip을 조절 할 수있다. 마우스 휠을 통해 fov를 조절 할 수 있다.

	> (vi). 정면도-측면도-상면도에 해당하는 카메라는 노란색 박스가 깜빡인다. 주 뷰포드 우측에 각 카메라의 뷰가 표시된다. ((iv)와 같이 'p'키를 통해 뷰를 바꿀 수 있다.)

	> (vii). 현재 서브 뷰포트에 표시중인 카메라의 카메라의 뷰잉볼륨은 'o'키를 통해 on/off 할 수 있다.
