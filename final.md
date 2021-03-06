# IOT 프로그래밍 2조 - 최종 발표 보고서

팀장: 김진상<br>팀원: 김동현 

발표일 : 2022년 5월 30일<br>발표자: 김진상

주제 : 1 VS 1 카드 뒤집기 게임

# 1. 진행 일정 및 주요 내용

- ## 1주차 활동 내용
  - 기존의 프로젝트 조사 : 리듬게임, 폭탄 해체 게임
  - 주제 선정 : 전체적인 진행 방식과 기능
  - 기존의 프로젝트와 차별성 제시 


- ## 2주차 활동 내용
  - 주제 변경 : 카드 뒤집기 게임으로 변경 (리듬 게임의 핵심인 부저가 없음)
  - 기존 프로젝트를 타겟시스템으로 이동하는 방법 학습 및 정상 작동 확인
  - 타겟 시스템의 입출력 장치 조작 방법 학습  



- ## 3주차 활동 내용
  - 알고리즘 순서도 작성 : 
  
  ![알고리즘 순서도](https://user-images.githubusercontent.com/80252681/171007720-70311687-6b1d-4270-8f1a-05c5568c8702.jpg)
  
  - PC에서 작동하는 프로그램 작성
  - 입출력 장치와 상호작용이 필요한 부분 계획
  - 추가적으로 구현할 기능 



- ## 4주차 활동 내용
  - 타겟 시스템에서 입출력장치와 상호작용하도록 프로그램 작성
  - 추가적으로 필요한 기능들 구현 작업
  - 타겟 시스템으로 프로그램을 설치하여 최종 작동 확인

- ## 최종 발표 전 활동 내용
  - 추가 기능 구상 및 구현
  - 안정적 작동 확인 및 정리



# 2. 구현된 기능 및 실제 작동

  ## 2.1 작동 순서
  1) Tact Switch를 이용하여 플레이어1과 플레이어2의 이름을 순서대로 입력 받습니다.
  2) 랜덤으로 카드 섞어 Dot matrix에 4x3 모양으로 배치합니다.
  3) player1부터 2개의 카드를 선택합니다.
  4) 맞추면 1점을 획득하고 턴은 유지됩니다.
  5) 틀리면 상대방에게 턴을 넘겨줍니다.
  6) 제한된 시간을 초과한 경우도 턴을 넘겨 줍니다.
  7) 6쌍의 카드가 모두 맞춰 질 때 까지 4,5,6을 반복합니다.
  8) 6쌍이 맞춰지면 각 플레이어가 획득한 점수에 따라 승자가 출력됩니다.

  ## 2.2 구현된 기능
  
  - 누구의 이름을 입력하는지 알 수 있는 clcd 기능
  
  ![1](https://user-images.githubusercontent.com/80252681/172615594-f32a0951-d14e-4526-9f9d-5b61c94253bc.jpg)
  ![1](https://user-images.githubusercontent.com/80252681/172616486-b2975aef-7991-483b-9912-fba602d75697.jpg)

  - dot matrix, clcd와 tact swich를 이용하여 player들의 이름을 입력받는 기능
  
  ![a1](https://user-images.githubusercontent.com/80252681/172620831-e6323cd4-e622-4043-8295-84cb75b5453c.jpg)
  ![b1](https://user-images.githubusercontent.com/80252681/172620836-c8967472-977b-487c-8930-2a0447dc2560.jpg)
  ![c1](https://user-images.githubusercontent.com/80252681/172620837-a35f986d-e19d-401c-97ec-476f6517100c.jpg)
  ![d1](https://user-images.githubusercontent.com/80252681/172620842-317b70eb-1b1b-4b58-b205-670c3b7965ca.jpg)
  ![e1](https://user-images.githubusercontent.com/80252681/172620846-1f704626-c07e-4754-aa84-b42eae72d6c4.jpg)
  
  - 현재 점수 상황을 보여주는 clcd 기능
  - 이름 입력의 진행 단계를 알 수 있는 led 기능
  - dot matrix에 4x3 카드 랜덤 배치 기능
  - tact switch를 이용하여 원하는 카드를 선택할 수 있는 기능
  - 카드를 고르면 고른 카드의 숫자를 dot matrix에서 확인 가능한 기능
  - dot matrix에서 고른 2개의 카드가 맞는지 아닌지 출력되는 기능( 표정 출력)
  - 실시간 점수가 반영된 lcd 기능 
  - 해당 턴의 타이머가 나타나는 FND 기능
  - 현재 누구의 턴인지를 알 수 있는 led 기능
  - 짝이 맞춰진 카드 쌍은 실시간으로 dot matrix에서 사라지는 기능 
  - 6쌍이 맞춰지면 승자가 lcd에 출력되는 기능
  - dot matrix에 전광판 출력되는 기능


  ## 2. 3 작동 실행 영상


# 3. 독창성, 창의성, 독창성
  - 동시에 4개의 장치에 접근하는 기능 
  - 타이머 시간 측정 부분
  - 스코어 판 문자열 만드는 거
  - 여러가지 예외 처리 ( 중복 선택 및 뒤집어진 카드 선택 )?

# 4. 참고 자료 
