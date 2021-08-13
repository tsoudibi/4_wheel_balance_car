window.onload = function(){


        function getClassName(tagName, classname) {
            if (document.getElementsByClassName) {
                return document.getElementsByClassName(classname);
            } else {
                var results = [];
                var elems = document.getElementsByTagName('*');
                for (var i = 0; i < elems.length; i++) {
                    if (elems[i].className.indexOf(classname) != -1) {
                        results[results.length] = elems[i];
                    }
                }
                return results;
            }
        }

        var turntable = getClassName('ul','turntable')[0];
        var result = getClassName('p','result')[0];

        function CreateParameter (turntableDom,resultDom){
            //參數
            this.turntable = turntableDom; //轉盤dom
            this.result = resultDom; //結果dom
            this.flag = true; //開關設置
            this.times = 20; //執行時間
            this.turns = Math.ceil(Math.random()*3+1); //旋轉圈數
            this.speed = Math.floor(Math.random()*5)+3; //速度
            this.turnNum = 12; //格子總數
            this.deg = 360/this.turnNum; //轉盤所對應度數
            this.initital = 0; //轉盤對應角度
            this.turnBuffer = this.deg/2-5; //每個格子對應的度數緩衝區
            this.num = Math.ceil(Math.random() * this.turnNum)-1; //隨機抽取的位置
            this.MathNum = 14; //重新編排數字與轉盤對應，14是因為i=1時已經减去了一個
            this.arr =  this.NewArr(this.MathNum,this.deg,this.turnBuffer); //轉盤角度參數
            this.initialDegMini = this.turns*360+this.arr[this.num][2]; //初始最小值度數
            this.initialDegMax = this.turns*360+this.arr[this.num][1]; //初始最大值度數
            this.MathAngle = Math.ceil(Math.random()*(this.initialDegMax-this.initialDegMini) )+this.initialDegMini; //轉盤停止的角度
            this.text ='結果為：'+ this.arr[this.num][0];

            console.log(this.arr[this.num])
            console.log(this.speed)
        }

        CreateParameter.prototype.NewArr = function (MathNum,deg,turnBuffer){
            //計算轉盤的各個角度參數
            var arr = [];
            for(let i = 1;i<=this.turnNum;i++){
                let num = MathNum-i; //做倒敘,跳過1
                if(i==1){num = i}
                let turnDeg = deg*i-deg;
                arr.push([num,turnDeg+turnBuffer,turnDeg-turnBuffer]) ;
            }
            return arr;
        }

        CreateParameter.prototype.OperatingDom = function(dom){
            //dom節點操作
            if(dom == 'rotate'){
                this.turntable.style.transform ="rotate("+this.initital+"deg)";
            }

            if(dom == 'innerHTML'){
                this.result.innerHTML = this.text;
            }

        }

        CreateParameter.prototype.judgment = function(){
            //判斷
            if(this.initital >= this.initialDegMini-420){

                if(this.speed>0.9){
                    this.speed = this.speed-0.05;
                }

            }

            if(this.initital >= this.MathAngle ){
                this.OperatingDom('innerHTML')
                this.reset();
            }else{
                //setTimeout内部指針會混亂所以需要外部定義
                var _this = this;
                setTimeout(function(){
                    _this.star()
                },this.times)
            }
        }

        CreateParameter.prototype.reset = function (){
            //重置
            this.initital = this.MathAngle-(parseInt(this.MathAngle/360)*360);
            this.OperatingDom('rotate')
            this.num =  Math.ceil(Math.random()*12)-1;
            this.turns = Math.ceil(Math.random()*5+1);
            this.speed = Math.floor(Math.random()*3)+3;
            this.initialDegMini = this.turns*360+this.arr[this.num][2];
            this.initialDegMax = this.turns*360+this.arr[this.num][1];
            this.MathAngle = Math.ceil(Math.random()*(this.initialDegMax-this.initialDegMini) )+this.initialDegMini;
            this.flag = true;
            this.text ='結果為：'+ this.arr[this.innerHTML][0];

        }

        CreateParameter.prototype.star = function(){
            this.OperatingDom('rotate'); //讓轉盤旋轉
            this.initital+=this.speed; //增加角度
            this.judgment(); //運行判斷
        }

        var ProxySingleParameter = (function(){

            var  instance =  new CreateParameter(turntable,result); //儲存參數
            var flag = instance.flag; //開關判斷是否正在運行中

            return function (turntable,result){
                if(!flag){
                    instance = new CreateParameter(turntable,result); //更新参数
                   console.log(instance)
                }
                return instance;
            }

        })()
        document.onclick = function(e){
            var target = e.target || e.srcElement;
            if(target.className == 'internal'){
                let Parameter = new ProxySingleParameter(turntable,result);
                if(Parameter.flag){
                    Parameter.result.classList.remove('none');
                    Parameter.star();
                    Parameter.flag = false;
                }else{
                    console.log(Parameter.arr[Parameter.num]);
                }
            }
        }
}
