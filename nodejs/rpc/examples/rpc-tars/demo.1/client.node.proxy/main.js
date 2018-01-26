var TarsClient  = require("../../../../protal.js").client;
var TarsProxy = require("./NodeTarsProxy").tars;

//ʹ�������ļ���ʼ��ͨ����
TarsClient.initialize("./config.conf");

//���ɴ�����
//var prx = TarsClient.stringToProxy(TarsProxy.NodeTarsProxy, TarsClient.configure.get("main.DevServer"));        //�ͻ��˺ͷ���˶�������IDC�������ͻ���ͨ�����أ���ѯ��б�Ȼ����÷����
//var prx = TarsClient.stringToProxy(TarsProxy.NodeTarsProxy, TarsClient.configure.get("main.ProxyServer"));      //�ͻ��˲����ڱ��أ�����˲�����IDC�������ͻ���ͨ�����ر����������IDC����
var servant=TarsClient.configure.get("main.LocalServer");
var prx = TarsClient.stringToProxy(TarsProxy.NodeTarsProxy,servant);      //�ͻ��˺ͷ���˶������ڱ��ء��ͻ���ֱ�����ط���

//���ɻص���
var success = function (result) {
    console.log("remote server endpoint:",             result.request.RemoteEndpoint.toString());   //���ε��ö�Ӧ�ķ���˵�ַ������Ϊ['tars-utils'].Endpoint
    console.log("result.response.costtime:",           result.response.costtime);                   //���ص��õĺ�ʱ����λΪ����
    console.log("result.response.return: ",            result.response.return);                     //�����ķ���ֵ����tars�ļ��ж��壩
    console.log("result.response.arguments.sValue1:",  result.response.arguments.sValue1);          //������out��������tars�ļ��ж��壩
    console.log("result.response.arguments.sValue1:",  result.response.arguments.sValue2);          //������out��������tars�ļ��ж��壩
}

var error = function (result) {
    console.log("result.response.error.code: ",        result.response.error.code);                 //���ε���ʧ�ܵĴ������
    console.log("result.response.error.message: ",     result.response.error.message);              //���ε���ʧ�ܵĴ�������
}

//���ýӿ�
prx.getUsrName("czzou").then(success, error).done();
