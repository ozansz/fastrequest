import os
import fastrequest.http

def test_basic_get():
    return fastrequest.http.get("https://www.google.com/")

def test_with_urlparams():
    return fastrequest.http.get("https://postman-echo.com/get?foo1=bar1&foo2=bar2")

def test_with_headers():
    return fastrequest.http.get("https://www.google.com/", {
        "X-FR-Test": "v1.2.6",
        "X-Hello": "World"
    })

def test_with_params_and_headers():
    return fastrequest.http.get("https://postman-echo.com/get?foo1=bar1&foo2=bar2", {
        "X-FR-Test": "v1.2.6",
        "X-Hello": "World"
    })

def test_save_response_to_file():
    save_file = "__test_save.data"

    res = fastrequest.http.get("https://postman-echo.com/get?foo1=bar1&foo2=bar2", {
        "X-FR-Test": "v1.2.6",
        "X-Hello": "World"
    })

    res.save_to(save_file)

    with open(save_file, "r") as fp:
        file_contents = fp.read()

        if file_contents != res.text():
            print("[DEBUG] File content:", repr(file_contents))
            print("[DEBUG] Resp content:", repr(res.text()))
            
            raise RuntimeError("Saved content does not match with response text.")

    os.remove(save_file)

if __name__ == "__main__":
    tests = [
        test_basic_get,
        test_with_headers,
        test_with_urlparams,
        test_with_params_and_headers,
        test_save_response_to_file
    ]

    for testfunc in tests:
        print(".", end="", flush=True)

        try:
            testfunc()
        except Exception as e:
            print(f"\n[!!] Caught exception in '{testfunc.__name__}':", e)
            exit(1)

    print("\n[+] All tests passed!")