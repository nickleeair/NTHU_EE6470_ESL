Github link：https://github.com/nickleeair/NTHU_EE6470_ESL/tree/main/HW1
1. Introduction of the Problem and my Solution endl/
這次的作業是要實作Median filter和Mean filter的演算法，並且要求兩種不同的Pixel transfer版本。
第一種是基於lab02-Sobel Filter的範例，單純利用FIFO Channels來做。
在計算Convolution時，一次僅將「必要」的「一個」Pixel送入Filter內做運算。
第二種是內部有Row-Buffer來暫存Row Pixels的版本。
但因爲老師要求row_buffer僅能儲存18pixels。在計算Convolution前，會先一次讀入18 Pixels，並利用這三個存在Filter內（local）的buffer做運算。
2. Implementation details
在Median filter中，實作方法是直接取得input的中位數。首先將input資料load進row中，再做sorting，再將中位數輸出。
在Mean filter中，Kernel的定義方法是從老師提供的資料。不論是版本一或是版本二的運算都是3x3 convolution的運算，將pixel讀入filter後只要將3x3 convolution的點乘完成並且加總之後除以總數10就可以得到一個pixel的輸出結果。
要特別注意的是，由希望輸出圖片尺寸與輸入圖片相同，故在做pixel讀取的時候，會將width以及height左右各多加一單位，並利用判斷式來判斷當前讀取的地方是否為圖片邊緣， 若是的話就會選擇將輸入filter的pixel data設成0，如此來實現zero-padding（如下圖）。
 ![image](https://user-images.githubusercontent.com/102891463/226709078-f38bc85a-e137-4368-b766-7563e2ece37d.png)

3. Additional features of your design and models
在Row-Buffer Based的實現中，要特別注意的一點是，由於我只開3個buffer來儲存15個Pixel的pixel data。所以說當前運算使用的data以及下一次新讀進的Row data都僅能在這3個Buffer內做操作。
而實現的辦法是：先定義Row[0]、Row[1]、Row[2]分別為由上往下的第一層～第三層，而只要當前3個Row的卷積運算完畢，則我會「依序」將Row[1]先shift給Row[0]，然後Row[2]shift給Row[1]，最後只要再將新讀進的Row data存放在R[2]就可以成功完成以下三個動作：1.保留舊的Row data 2.重新排列Row-Buffer 3.存入新的Row data。而重複上述操作就可以完成整張圖片的Convolution運算而不需要開設過多的Buffer或者是重複讀取多次相同的pixel data(如下圖)。
 ![image](https://user-images.githubusercontent.com/102891463/226709142-7ac56645-e723-402c-9bea-b8c276b6ac2a.png)

4. Experimental results
The original picture:
![image](https://user-images.githubusercontent.com/102891463/226709268-31a14eca-1fb0-4284-a556-364ac274311a.png)
![image](https://user-images.githubusercontent.com/102891463/226709290-115b3cf7-23b2-4e5b-9686-0732dbc5e717.png)

The blur picture generated from filters(版本一跟版本二的Kernel參數選用一樣，故成像一樣）
   ![image](https://user-images.githubusercontent.com/102891463/226709397-6938a853-56b6-46bf-8184-6193c46a88e0.png)
![image](https://user-images.githubusercontent.com/102891463/226709415-8c53077a-b04c-419d-94f2-a97b5a013d90.png)

5. Discussions and conclusions
The Simulation time and the Pixels transfer times(from tb to filter) of wFIFO version:
 ![image](https://user-images.githubusercontent.com/102891463/226709431-b0a964af-84ea-4dd8-942e-e6a4c0cbe188.png)

The Simulation time and the Pixels transfer times(from tb to filter) of Row-Based version:
 ![image](https://user-images.githubusercontent.com/102891463/226709445-4dab16b1-c859-481b-8243-d064b02c2180.png)

在第一種wFIFO版本的架構中，Filter在讀取資料的時候每次僅會讀取3x3 convolution內的其中一個pixel進來做點乘運算，
所以當3x3 convolution stride=1 往右滑的時候，下一次3x3 convolution運算讀取都會「重複讀到」上一次3x3 convolution運算時就已經使用過的6個pixel data。
而第二種Row-Buffer Based的架構中，由於Filter會一次讀進3個Row的pixel data並且「暫存」起來，當3x3 convolution往右滑時，
所有的資料都已經存在Row-Buffer內，故「不會」有重複讀取pixel data的問題。
由上圖的模擬結果來看，版本一在Pixel transfer times from testbench to filter的次數為「2359296」次，而版本二則僅有「264196」次。
版本二新增的Row-Buffer大幅的降低了Memory與CPU間的資料傳輸要求，這可以讓Power consumption的efficiency有大幅的改善。
由於程式內部的wait(delay_time)目前僅僅只是用來模擬behavior而已，不帶有實際上特別的意義(delay_time可以任意改動)，
所以我認爲用Pixel transfer times來參考更有意義。
