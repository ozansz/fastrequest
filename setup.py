from setuptools import Extension, setup

http_module = Extension(
    "fastrequest.http",
    sources=[
        'fastrequest/http/http.c',
        'fastrequest/http/api.c',
        'fastrequest/http/stringbuffer.c',
        'fastrequest/http/utils.c',
        'fastrequest/http/httpresponse.c',
        'fastrequest/http/httprequest.c',
        'fastrequest/http/funcwrappers.c',
    ],
    library_dirs=['/usr/local/lib'],
    libraries=['curl']
)

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name="fastrequest",
    version="1.6.3-beta",
    author="Ozan Sazak",
    author_email="ozan.sazak@protonmail.ch",
    description="fastrequest fast http requests extension",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/ozansz/fastrequest",
    packages=['fastrequest'],
    ext_modules=[http_module],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3',
)
