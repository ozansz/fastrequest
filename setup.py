from setuptools import Extension, setup

module = Extension(
    "fastrequest",
    sources=[
        'src/fastrequest.c',
        'src/api.c',
        'src/stringbuffer.c',
        'src/utils.c'
    ],
    library_dirs=['/usr/local/lib'],
    libraries=['curl']
)

setup(
    name="fastrequest",
    version="1.0.0-beta",
    description="fastrequest fast http requests extension",
    ext_modules=[module]
)
