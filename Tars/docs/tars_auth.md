# tars��Ȩ����

tars��һ��ǿ���RPC��ܣ���ֻ��Ҫ�õ������÷���tarsЭ���ļ����Ϳ���ʹ��tars��ܱ��룬���󱻵������񡣷����ͬʱҲ������һЩ��ȫ������������ǵķ���ӿ������еģ���ϣ���������ݱ�����������ʡ�����������Ҫһ��ϸ��������ļ�Ȩ���ʻ��ơ�
tars�ṩ�������Ļ��ƣ���ֻ��Ҫ�����˺����룬tars�����������Զ����������֤����ҵ�����ȫ͸���������޷��ṩ��ȷ�˺�����
�Ŀͻ��ˣ�ֱ���ߵ����ӡ�

Ŀǰֻ��C++֧�ּ�Ȩ���ܣ�java����֧�֡�

## tars��Ȩʹ��

tars��Ȩ��ʹ�÷ǳ��򵥣��㲻��Ҫ�޸�һ�д��룬ֻ��Ҫ���������ļ���
ֻ��Ҫ�������������������ã�

### 1. �޸ı����÷���endpoint��������Ȩ����
��tars����ƽ̨��ѡ����Ҫ���ü�Ȩ�ķ��񣬱༭servant���޸�endpoint�����-e 1.
-e���������ʾ�Ƿ�����Ȩ��Ĭ��Ϊ0�ǲ������ģ�Ϊ1���ǿ�����

![endpoint](images/tars_auth_endpoint.png)

### 2. �޸ı����÷��������ļ�����˺�����
ע�⣬�����÷������ж��adapter��ÿ����Ҫ��Ȩ��adapter�����Լ��������˺����롣
��ͼ����test.oko.BertObjAdapter�����˺�bert������123456��

![server](images/tars_auth_server.png)

���ڽ�ͼ��ȫ��ʵ��˽��ģ���������£�

```xml
<tars>
  <application>
    <server>
        <test.oko.BertObjAdapter>
            accesskey=bert
            secretkey=123456
        </test.oko.BertObjAdapter>
    </server>
  </application>
</tars>
```

���ڣ���webƽ̨��������

### 3. �޸ĵ��÷��������ļ�����˺�����
ע�⣬���÷����Է��ʶ��obj��ÿ����Ҫ��Ȩ��obj�����Լ��������˺����롣
��ͼ����test.oko.BertObj�����˺�bert������123456��

![client](images/tars_auth_client.png)

���ڽ�ͼ��ȫ��ʵ��˽��ģ���������£�

```xml
<tars>
  <application>
    <client>
        <test.oko.BertObj>
            accesskey=bert
            secretkey=123456
        </test.oko.BertObj>
    </client>
  </application>
</tars>
```

���ڣ���webƽ̨�������÷���

��������ķ���ͱ���ͨ���˺�����ſ��Է����ˡ�

