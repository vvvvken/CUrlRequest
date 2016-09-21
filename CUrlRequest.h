#ifndef __CURLREQUEST_HPP__
#define __CURLREQUEST_HPP__

#include <curl/curl.h> 
#include <memory>
#include <functional>
#include <string>
#include <vector>


using namespace std;

typedef size_t(*CURL_WRITEFUNCTION_PTR)(void *buffer, size_t size, size_t count, void *data);

typedef enum  {
	STRING_TYPE,
	FILE_TYPE
} PARAMETER_TYPE;

typedef pair<pair<string, PARAMETER_TYPE>, string> Parameter;

typedef vector<Parameter> Parameters;
typedef vector<string> Headers;

typedef CURL* PCURL;

class CUrlRequest
{
public:
	CUrlRequest();
	void setCookieFile(const string & cookie_file);
	void setCA(const string & ca_path, const string & ca_info);

	void get(string url, function<void(const char*body, size_t len)> func_finish, function<void(const int code)> failt_callback);
	void get(string url, const Headers & headers, function<void(const char*body, size_t len)> func_finish, function<void(const int code)> failt_callback);

	void post(string url, const Parameters &parameters, function<void(const char*body, size_t len)> func_finish, function<void(const int code)> failt_callback);
	void post(string url, const Headers &headers, const Parameters &parameters, function<void(const char*body, size_t len)> func_finish, function<void(const int code)> failt_callback);

	void post(string url, const string & json, function<void(const char*body, size_t len)> func_finish, function<void(const int code)> failt_callback);
	void post(string url, const Headers &headers, const string &json, function<void(const char*body, size_t len)> func_finish, function<void(const int code)> failt_callback);

	virtual ~CUrlRequest();

private:
	shared_ptr<CURL> _pCUrl;
	CURLcode _res;

	string _ca_path;  //ca证书路径
	string _ca_info;   //ca证书文件名
	string _cookie_file; //cookie文件路径

};

#endif //__CURLREQUEST_HPP__
