#CUrlRequest
Khan.Lau 大神亲情力作，VC下对curl的封装
***
##Usage
<pre>
CUrlRequest urq;urq.setCookieFile("conf/cookie.txt");urq.setCA("conf/", "cacert.pem");string url = UnicodeToANSI(GlobalVar::getInstance().url + L"/daemon/filtered_party");Parameters parameters;char str[64] = { 0 };sprintf_s(str, "%llu", timestamp);parameters.push_back(make_pair(make_pair("search", STRING_TYPE), str));
urq.post(url, parameters, [hWnd](const char* body, size_t len)->void {                        }, 
                        [hWnd](int code)->void {			}
</pre>

##Other
std破天荒的引入了新的字符编码工具
<pre>
std::wstring to_wchar_t(std::string str) {	return std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t >{}.from_bytes(str);}std::string to_utf8(std::wstring wstr) {	return std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t >{}.to_bytes(wstr);}
</pre>