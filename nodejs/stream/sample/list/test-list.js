var TarsStream = require("../../TarsStream");

//STEP01 - 声明数组
var list = new TarsStream.List(TarsStream.String);

//STEP02 - 赋值
for (var i = 0; i < 10; i++) {
    list.push("LIST." + i.toString());
}

//STEP03 - 遍历
for (var i = 0, len = list.length; i < len; i++) {
	console.log(list.value[i]);
}

list.forEach(function (value, index){
    console.log("forEach:", value, index);
});

//STEP04 - 获取数组的大小
console.log("ARRAY.LENGTH =", list.length);

//TARS编解码示例

var os = new TarsStream.OutputStream();
os.writeList(1, list);

var is = new TarsStream.InputStream(os.getBinBuffer());
var ta = is.readList(1, true, TarsStream.List(TarsStream.String));

ta.forEach(function (value, index){
    console.log("Read forEach:", value, index);
});