#include <iterator>
#include <algorithm>
#include <mutex>
#include "UrlRequest.h"

static std::once_flag flag;

CUrlRequest::CUrlRequest() :_res{ CURLE_OK }
{
	std::call_once(flag, []() {
		curl_global_init(CURL_GLOBAL_ALL);
	});

	_pCUrl = shared_ptr<CURL>(curl_easy_init(), [](CURL* curl)->void {
		curl_easy_cleanup(curl);
		curl = NULL;
	});
}

void CUrlRequest::setCookieFile(const string &cookie_file) {
	this->_cookie_file = cookie_file;
}

void CUrlRequest::setCA(const string &ca_path, const string &ca_info) {
	this->_ca_path = ca_path;
	this->_ca_info = ca_info;
}


void CUrlRequest::get(string url, function<void(const char* body, size_t len)> func_finish, function<void(const int code)> failt_callback) {
	this->get(url, Headers(), func_finish, failt_callback);
}



void CUrlRequest::get(string url, const Headers & headers, function<void(const char* body, size_t len)> func_finish, function<void(const int code)> failt_callback) {
	vector<char> data;
	CURL * curl = _pCUrl.get();
	if (curl != NULL) {
		struct curl_slist * headerlist = NULL;
		for (string header : headers) {
			headerlist = curl_slist_append(headerlist, header.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);//设置CURLOPT_WRITEFUNCTION的最后一个参数值

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, static_cast<CURL_WRITEFUNCTION_PTR>([](void *buffer, size_t size, size_t count, void *data)->size_t {
			size_t n = size * count;
			vector<char> *vec = (vector<char> *)data;
			char * pstr = (char*)buffer;

			std::copy(pstr, pstr + n, std::back_inserter(*vec));
			TRACE("CURL_WRITEFUNCTION_PTR\n");
			return n;
		}));

		if (this->_cookie_file != "") {
			curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1L);
			curl_easy_setopt(curl, CURLOPT_COOKIEFILE, this->_cookie_file);
		} else {
			curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 0L);
		}


		string tmp;
		string https("https");
		std::transform(url.begin(), url.end(), std::back_inserter(tmp), tolower);

		if (tmp.compare(0, https.size(), https) == 0) {
			if (this->_ca_path != "" && this->_ca_info != "") {
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

				curl_easy_setopt(curl, CURLOPT_CAPATH, this->_ca_path);
				curl_easy_setopt(curl, CURLOPT_CAINFO, this->_ca_info);

				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
			} else {
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			}

		}

		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);

		_res = curl_easy_perform(curl);

		if (_res != CURLE_OK) {
			failt_callback(_res);
			TRACE(L"%s", curl_easy_strerror(_res));
		} else {
			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			if (http_code == 200 || http_code == 304) {
				string str(data.begin(), data.end());
				func_finish(str.c_str(), str.length());
			} else {
				failt_callback(http_code);
			}
			//curl_easy_cleanup(curl);
		}
		if (headerlist != NULL) curl_slist_free_all(headerlist);
	}
}



void CUrlRequest::post(string url, const Parameters &parameters, function<void(const char* body, size_t len)> func_finish, function<void(const int code)> failt_callback) {
	this->post(url, Headers(), parameters, func_finish, failt_callback);
}

void CUrlRequest::post(string url, const Headers & headers, const Parameters & parameters, function<void(const char*body, size_t len)> func_finish, function<void(const int code)> failt_callback) {

	vector<char> data;
	CURL * curl = _pCUrl.get();
	if (curl != NULL) {
		struct curl_httppost * formpost = NULL;
		struct curl_httppost * lastptr = NULL;

		struct curl_slist * headerlist = NULL;

		for (Parameter item : parameters) {
			PARAMETER_TYPE type = item.first.second;
			const string &key = item.first.first;
			const string &val = item.second;

			if (type == STRING_TYPE) {
				curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, key.c_str(), CURLFORM_COPYCONTENTS, val.c_str(), CURLFORM_END);
			} else if (type == FILE_TYPE) {
				curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, key.c_str(), CURLFORM_FILE, val.c_str(), CURLFORM_END);
			} else {

			}
		}
		
		for (string header : headers) {
			headerlist = curl_slist_append(headerlist, header.c_str());
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);//设置CURLOPT_WRITEFUNCTION的最后一个参数值

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, static_cast<CURL_WRITEFUNCTION_PTR>([](void *buffer, size_t size, size_t count, void *data)->size_t {
			size_t n = size * count;
			vector<char> *vec = (vector<char> *)data;
			char * pstr = (char*)buffer;

			std::copy(pstr, pstr + n, std::back_inserter(*vec));
			TRACE("CURL_WRITEFUNCTION_PTR\n");
			return n;
		}));

		if (this->_cookie_file != "") {
			curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1L);
			curl_easy_setopt(curl, CURLOPT_COOKIEFILE, this->_cookie_file);
		} else {
			curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 0L);
		}


		string tmp;
		string https("https");
		std::transform(url.begin(), url.end(), std::back_inserter(tmp), tolower);

		if (tmp.compare(0, https.size(), https) == 0) {
			if (this->_ca_path != "" && this->_ca_info != "") {
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

				curl_easy_setopt(curl, CURLOPT_CAPATH, this->_ca_path);
				curl_easy_setopt(curl, CURLOPT_CAINFO, this->_ca_info);

				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
			} else {
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			}

		}

		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);

		_res = curl_easy_perform(curl);

		if (_res != CURLE_OK) {
			failt_callback(_res);

			TRACE(L"%s", curl_easy_strerror(_res));

		} else {
			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			if (http_code == 200 || http_code == 304) {
				string str(data.begin(), data.end());
				func_finish(str.c_str(), str.length());
			} else {
				failt_callback(http_code);
			}
		}
		if (formpost != NULL) curl_formfree(formpost);
		if (headerlist != NULL) curl_slist_free_all(headerlist);
	}

}


CUrlRequest::~CUrlRequest()
{
}