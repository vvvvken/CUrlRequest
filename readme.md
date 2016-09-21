#CUrlRequest
对curl的封装
***
##Usage
<pre>
CUrlRequest urq;
urq.setCookieFile("conf/cookie.txt");
urq.setCA("conf/", "cacert.pem");

string url = "https://xxx.com/daemon/filtered_party");
Parameters parameters;

char str[64] = { 0 };
sprintf_s(str, "%llu", timestamp);

parameters.push_back(make_pair(make_pair("search", STRING_TYPE), str));
urq.post(url, parameters, [hWnd](const char* body, size_t len)->void {

},  [hWnd](int errorcode)->void {

});
</pre>

