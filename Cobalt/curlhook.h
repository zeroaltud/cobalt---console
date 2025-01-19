#pragma once

#include <Windows.h>
#include <regex>

#include "memcury.h"
#include "settings.h"

#include "../vendor/cURL/curl.h"
#include "url.h"

inline CURLcode(*CurlSetOpt)(struct Curl_easy*, CURLoption, va_list) = nullptr;
inline CURLcode(*CurlEasySetOpt)(struct Curl_easy*, CURLoption, ...) = nullptr;

inline CURLcode CurlSetOpt_(struct Curl_easy* data, CURLoption option, ...)
{
	va_list arg;
	va_start(arg, option);

	const auto result = CurlSetOpt(data, option, arg);

	va_end(arg);
	return result;
}

std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

#define XOR

inline CURLcode CurlEasySetOptDetour(struct Curl_easy* data, CURLoption tag, ...)
{
	va_list arg;
	va_start(arg, tag);

	CURLcode result = {};

	if (!data) return CURLE_BAD_FUNCTION_ARGUMENT;

	//Ssl bypass
	if (tag == CURLOPT_SSL_VERIFYPEER)
	{
		result = CurlSetOpt_(data, tag, 0);
	}

	//Disable Proxy
	/* if (tag == CURLOPT_PROXY)
	{
		result = CurlSetOpt_(data, tag, "");
	} */

	//URL redirection
	else if (tag == CURLOPT_URL)
	{
		std::string url = va_arg(arg, char*);

		Uri uri = Uri::Parse(url);

		if (bIsS13Epic)
		{
			if (!uri.QueryString.empty())
			{
				// this *should* work
				//std::cout << "before query: " << uri.QueryString << '\n';
				uri.QueryString = ReplaceString(uri.QueryString.data(), "Windows", "IOS");
				//std::cout << "after query: " << uri.QueryString << '\n';
			}
			else
			{
				//std::cout << "Empty query!\n";
			}
		}

		//std::cout << "URL: " << uri.Host << uri.Path << '\n';

#if defined(URL_HOST) && defined(URL_PROTOCOL) && defined(URL_PORT)
		if (uri.Host.ends_with(XOR("ol.epicgames.com"))
			|| uri.Host.ends_with(XOR("epicgames.dev")) // wooo eos
			|| uri.Host.ends_with(XOR("ol.epicgames.net")) // i forgor what endpoint this was for
			|| uri.Host.ends_with(XOR(".akamaized.net"))
			|| uri.Host.ends_with(XOR("on.epicgames.com"))
			|| uri.Host.ends_with(XOR("game-social.epicgames.com"))
			|| uri.Host.contains(XOR("superawesome.com"))
			|| uri.Host.contains(XOR("ak.epicgames.com")))
		{
			if (CobaltUsage == ECobaltUsage::Private)
			{
				url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
			}
			else if (CobaltUsage == ECobaltUsage::Hybrid)
			{
				if (uri.Path.contains("/fortnite/api/v2/versioncheck/")) {
					url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
				}
				else if (uri.Path.contains("/fortnite/api/game/v2/profile")) {
					url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
				}
				else if (uri.Path.contains("/content/api/pages/fortnite-game")) {
					url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
				}
				else if (uri.Path.contains("/affiliate/api/public/affiliates/slug")) {
					url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
				}
				else if (uri.Path.contains("/socialban/api/public/v1")) {
					url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
				}
				else if (uri.Path.contains("/fortnite/api/cloudstorage/system")) {
					url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
				}
			}
		}

#endif

		result = CurlSetOpt_(data, tag, url.c_str());
	}	

	else
	{
		result = CurlSetOpt(data, tag, arg);
	}

	va_end(arg);
	return result;
}


__int64 sigscan(const std::string& Str) { return Memcury::Scanner::FindPattern(Str.c_str()).Get(); }