import json
from urllib.parse import urlencode, quote_plus

import fastrequest.http

test_query_params = {"foo1": "bar1", "foo2": "bar2"}
test_headers = {
        "X-FR-Test": "v1.2.6",
        "X-Hello": "World"
    }
test_json_payload = {
        "test": 123,
        "foo": {
            "bar": 46,
            "bar2": [3, 4, {}]
        }
    }

encode_f = lambda x: urlencode(x, quote_via=quote_plus)

def test_basic_post():
    return fastrequest.http.post("https://postman-echo.com/post")

def test_with_urlparams():
    res = fastrequest.http.post("https://postman-echo.com/post?" + encode_f(test_query_params))

    assert(encode_f(res.json()["args"]) == encode_f(test_query_params))

    return res

def test_with_payload_string():
    res = fastrequest.http.post("https://postman-echo.com/post", encode_f(test_query_params))

    assert(encode_f(res.json()["form"]) == encode_f(test_query_params))

    return res

def test_with_payload_json():
    res = fastrequest.http.post("https://postman-echo.com/post", test_json_payload)

    assert(json.dumps(res.json()["json"]) == json.dumps(test_json_payload))

    return res

def test_with_payload_custom():
    return fastrequest.http.post("https://postman-echo.com/post", [1, 2, 3, ("q")])

def test_with_headers():
    return fastrequest.http.post("https://postman-echo.com/post", "", test_headers)

def test_with_all():
    return fastrequest.http.post("https://postman-echo.com/post", test_json_payload, test_headers)

if __name__ == "__main__":
    tests = [
        test_basic_post,
        test_with_headers,
        test_with_urlparams,
        test_with_payload_string,
        test_with_payload_json,
        test_with_payload_custom,
        test_with_all
    ]

    for testfunc in tests:
        print(".", end="", flush=True)

        try:
            testfunc()
        except Exception as e:
            print(f"\n[!!] Caught exception in '{testfunc.__name__}':", e)
            exit(1)

    print("\n[+] All tests passed!")