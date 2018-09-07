from setuptools import Extension, setup

module = Extension("fastrequest", sources=['fastrequest.c', 'libfrext.c'],
    library_dirs=['/usr/local/lib'], libraries=['curl'])

setup(
    name="fastrequest",
    version="0.6.10-beta",
    description="fastrequest fast http requests extension",
    ext_modules=[module]
)
