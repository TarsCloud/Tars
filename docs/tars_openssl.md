# tars tlsͨ��֧��

OpenSSL��һ�����佡׳��������ȫ��TLS��SSLЭ��⣬Ҳ��һ��ͨ�õ������㷨�⡣��Ϊ���ǵ�ͨ���ṩ�˰�ȫ�Ժ������Ա�֤��tars���OpenSSL��֧��TLS��һ�����õ���Ҫ���ܡ�
TLSЭ���Ӧ�ò��HTTP��ϣ���֧����httpsЭ�顣
���⣬TLS���ܺ�[tars��Ȩ����](tars_auth.md)����ͬʱ���á�
Ŀǰֻ��C++֧��TLS���ܣ�java����֧�֡�

## tars tlsʹ��

tars tls��ʹ�÷ǳ��򵥣��㲻��Ҫ�޸�һ�д��룬ֻ��Ҫ���������ļ���
ֻ��Ҫ���������ĸ��������ã�

### 1. ����tars��ܣ�֧��tls
tars���Ĭ�ϲ�����tls�����ȸĶ�cpp/build/CMakeLists.txt������17��TARS_SSL�����Ϊ1��
```cpp
set(TARS_SSL 1)
```
�������±��밲װtars��ܡ�
�޸�ҵ��makefile�����һ�ж��壺
```cpp
TARS_SSL = 1
```
ע����һ��**һ��Ҫ����include /usr/local/tars/cpp/makefile/makefile.tars����**�����±�����ҵ������ϴ����ݲ�������

### 2. �޸ı����÷���endpointЭ�飬��tcp��udp��Ϊssl
��tars����ƽ̨��ѡ����Ӧ�ķ��񣬱༭servant���޸�endpointЭ�飬��ԭ�ȵ�tcp��udp��Ϊssl.

![endpoint](images/tars_ssl_endpoint.png)

### 3. �޸ı����÷��������ļ����֤������
һ����˵��TLSʹ�õ�����֤�����ͻ�����֤�����������Ա����÷�һ��Ҫ�ṩ֤�顣
���ٽ�ͼ�������֪����ô��˽��ģ�壬�鿴[tars��Ȩ](tars_auth.md)�еĽ�ͼ����˽��ģ��������������£�
```xml
<tars>
    <application>
        <serverssl>
           path=/certs/ # ֤������Ŀ¼
           ca=ca.pem # ca����֤�飬����֤�ͻ��˿��Բ���д
           verifyclient=0 # ����֤�ͻ���
           cert=server-cert.pem # ������public֤��
           key=server-key.pem # ������private֤��
        </serverssl>
    </application>
</tars>
```
���ڣ���webƽ̨��������

### 4. �޸ĵ��÷��������ļ����֤��
������������tars registry���Ի�ȡ�������ĵ�ַ�����Ա�����������ڵ�ַ�����ò���Ҫ�ģ�ֻҪ���֤��ͺ��ˡ�
һ����˵����������Ҫ�ṩ֤�飬����ֻҪ����һ��PKI�Ĺ�Կ֤��������֤�������Ϳ����ˡ����������Ҫ���������ṩ֤�飬�Ǿ���Ҫ�ṩ֤�飬��2���Ѿ������������ﲻ���ظ����޸�������˽��ģ�壬tls֤���������£�
```xml
<tars>
    <application>
        <clientssl>
           path=/certs/ # ֤������Ŀ¼
           ca=ca.pem # ca����֤�飬��֤������
        </clientssl>
    </application>
</tars>
```

���ڣ���webƽ̨�������÷���

û���޸�һ�д��루����Ҫ���±������ӣ�����ķ����Ѿ���ʼʹ��tls����ͨ���ˡ�

