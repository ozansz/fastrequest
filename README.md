# fastrequest
A Python C extension module for fast HTTP requests with SSL/TLS support

## Installation
To build and install the extension, you just need to run the `install.sh` script. 
The script then installs required packages, builds the extension using `setuptools` 
and then installs it using `pip`.

```shell
chmod +x install.sh
./install.sh
```

## Example Usage
Just import `fastrequest` module. It's usage is as easy as ABC!

```python
import fastrequest.http

res = fastrequest.http.get("https://www.google.com")
res.save_to("index.html")
```

Also, JSON (`application/json`) responses can be handled easily too.

```python
import json
import fastrequest.http

res = fastrequest.http.get("https://jsonplaceholder.typicode.com/comments?postId=1")

with open("data.json", "w") as fp:
  json.dump(fp, res.json())
 ```

## Module Reference

### HTTPResponse

HTTPResponse is the generic HTTP/HTTPS response object returned from the GET/POST functions.

Pythonic definition of HTTPResponse is as follows,

```Python
class HTTPResponse(object):
  def __init__(self, data, size, url=""):
    self.data = data
    self.size = size
    self.url = url

  def text(self):
    return self.data.decode()

  def json(self):
    return json.loads(self.text())
```

### http.get

`http.get(url: str[, headers:dict]) -> HTTPResponse`

HTTP/GET request function using `libcurl` as backend.

## API Reference

### HTTPResponse

HTTPResponse is the generic HTTP/HTTPS response object returned from the GET/POST functions.

The data structure of HTTPResponse object is as follows,

```C
typedef struct {
    PyObject_HEAD
    PyObject *response_size;
    PyObject *response_data;
    PyObject *request_url;
} HTTPResponseObject;
```

Where
* `response_size` is byte-size of the response body (PyLong)
* `response_body` is the response body (PyBytes)
* `request_url` is the requested URI (PyUnicode)

### StringBuffer

StringBuffer is the generic 'string' object implementation to be used in Fast Request API functions.

The data structure of StringBuffer object is as follows,

```C
typedef struct {
    char *buf;
    int_fast64_t index;
    int_fast64_t size;
} StringBuffer;
```

### FastRequestAPI_LibcurlHttpGet

`StringBuffer *FastRequestAPI_LibcurlHttpGet(char *url, PyObject *headers)`

Send HTTP/GET request to machine with the `url` specified with optional custom `headers`. This function uses `libcurl` as the backend.