var TarsStream = require("../../TarsStream");

//类型定义示例
var m = new TarsStream.Map(TarsStream.String, TarsStream.String);
m.put("a1", "value1");
m.put("a2", "value2");

//TarsStream编码示例
var os = new TarsStream.OutputStream();
os.writeMap(0, m);

//TarsStream解码示例
var data = os.getBinBuffer().toNodeBuffer(); //这里得到一个NodeJS内建的Buffer类型的变量

var is   = new TarsStream.InputStream(new TarsStream.BinBuffer(data));
var tmp  = is.readMap(0, true, TarsStream.Map(TarsStream.String, TarsStream.String));
tmp.forEach(function (key, value){
    console.log("key:", key, "value:", value)
});
