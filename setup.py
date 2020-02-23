from setuptools import Extension, setup

http_module = Extension(
    "fastrequest.http",
    sources=[
        'fastrequest/http/http.c',
        'fastrequest/http/api.c',
        'fastrequest/http/stringbuffer.c',
        'fastrequest/http/utils.c',
        'fastrequest/http/httpresponse.c',
        'fastrequest/http/httprequest.c'
    ],
    library_dirs=['/usr/local/lib'],
    libraries=['curl']
)

setup(
    name="fastrequest",
    version="1.5.0-beta",
    description="fastrequest fast http requests extension",
    packages=['fastrequest'],
    ext_modules=[http_module]
)
