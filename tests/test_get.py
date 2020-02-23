import fastrequest

def test_basic_get():
    return fastrequest.http_get("https://www.google.com/")

def test_with_urlparams():
    return fastrequest.http_get("https://postman-echo.com/get?foo1=bar1&foo2=bar2")

def test_with_headers():
    return fastrequest.http_get("https://www.google.com/", {
        "X-FR-Test": "v1.2.6",
        "X-Hello": "World"
    })

def test_with_params_and_headers():
    return fastrequest.http_get("https://postman-echo.com/get?foo1=bar1&foo2=bar2", {
        "X-FR-Test": "v1.2.6",
        "X-Hello": "World"
    })

if __name__ == "__main__":
    tests = [
        test_basic_get,
        test_with_headers,
        test_with_urlparams,
        test_with_params_and_headers
    ]

    for testfunc in tests:
        print(".", end="", flush=True)

        try:
            testfunc()
        except Exception as e:
            print(f"\n[!!] Caught exception in '{testfunc.__name__}':", e)
            exit(1)

    print("\n[+] All tests passed!")