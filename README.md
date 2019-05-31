# history
forked from HULOP project. 

updated by Donghyun Yoo

# BLE localization library
A BLE beacon-based localization library. This library is used as an iOS framework in NavCogIOSv3.

## Prerequisites
- [cereal version 1.1.2](http://uscilab.github.io/cereal/) (BSD License)
- [picojson version 1.3.0](https://github.com/kazuho/picojson) (BSD License)
- [Boost version 1.61.0](http://www.boost.org) (Boost Software License)
- [Eigen version 3.2.5 (EIGEN_MPL2_ONLY)](http://eigen.tuxfamily.org) (Mozilla Public License Version 2.0)
- [OpenCV version 3.2](http://opencv.org/) (BSD License)

## Build instructions
### Install CocoaPods (If you have not installed)
- Install and update Homebrew
> /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

- $brew install python
- $pip install mercurial
- Install CocoaPods
> sudo gem install cocoapods

### install cmake 3.13.5
- over 3.14 version doesn't work. don't install 3.14 with HomeBrew
- download binary file of 3.13.5 from https://cmake.org/download/. Install Cmake-gui and then open CMake-gui and Tools->How to install command line use


### Build bleloc Framework
- $git clone https://github.com/hulop/blelocpp.git
- $cd blelocpp/platform/ios
- $pod install
- $sh build.sh Release
- error


### Build BasicLocalizaer
Install opencv first
> brew install opencv

Then
>cd tools/BasicLocalizaer
>pod install
> sh build.sh Release



- if you have the following error,
xcode-select: error: tool 'xcodebuild' requires Xcode, but active developer directory '/Library/Developer/CommandLineTools' is a command line tools instance

> sudo xcode-select -s /Applications/Xcode.app/Contents/Developer


----
## About
[About HULOP](https://github.com/hulop/00Readme)

## License
[MIT](http://opensource.org/licenses/MIT)

## [README](https://raw.githubusercontent.com/hulop/blelocpp/master/README.txt)
This library is intended solely for use with an Apple iOS product and intended
to be used in conjunction with officially licensed Apple development tools.
