var TarsStream = require("../../tars");
var Ext = require("../tars/Ext.js").Ext;

var abcd = new TarsStream.BinBuffer();
abcd.writeInt32(12345678);

var temp = new Ext.ExtInfo();
temp.data.insert("abcd", abcd);

temp.cons.insert("mapa", temp.data);

var ost = new TarsStream.OutputStream();
ost.writeStruct(1, temp);

var ist = new TarsStream.InputStream(ost.getBinBuffer());
var tst = ist.readStruct(1, true, Ext.ExtInfo);

console.log("LEN:", tst.data.get("abcd").readInt32());

console.log("LEN:", tst.cons.get("mapa").get("abcd").readInt32());

