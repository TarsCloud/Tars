var TarsStream = require("../../tars");

var tup_encode = new TarsStream.Tup();
tup_encode.writeUInt32("uin",  155069599);
tup_encode.writeString("name", "KevinTian");
tup_encode.writeUInt32("gid",  1002121);
tup_encode.writeUInt32("num",  10);

var BinBuffer = tup_encode.encode(true);

var data = BinBuffer.toNodeBuffer();

var tup_decode = new TarsStream.Tup();
tup_decode.decode(new TarsStream.BinBuffer(data));

var num  = tup_decode.readUInt32("num");
var gid  = tup_decode.readUInt32("gid");
var name = tup_decode.readString("name");
var uin  = tup_decode.readUInt32("uin");

console.log("name:", name);
console.log("num :", num);
console.log("gid :", gid);
console.log("uin :", uin);
