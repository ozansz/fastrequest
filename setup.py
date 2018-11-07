from setuptools import Extension, setup

module = Extension("fastrequest", sources=['src/fastrequest.c', 'src/libfrext.c'],
    library_dirs=['/usr/local/lib'], libraries=['curl'])

setup(
    name="fastrequest",
    version="0.5-beta",
    description="fastrequest fast http requests extension",
    ext_modules=[module]
)
