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
import fastrequest

res = fastrequest.get("https://www.google.com")

with open("index.html", "w") as savefile:
  savefile.write(res)
 ```
