// Windows CLI app to interact with all the sound devices - either to list the available devices out to the console 
// OR to switch the current Sound Device to the device id passed in as a command line argument.
// command line:
// audio-dev-enumerate --list
// audio-dev-enumerate --device {}
// 
// e.g. on my machine I could use the following command line to change to a "RealTek" audio device with the device id found by calling the --list option.
// audio-dev-enumerate --device {0.0.0.00000000}.{e774d123-a2b9-4445-b182-5b1d3632c0d8}
//
// I compiled this and then used https://www.autohotkey.com/ to automate this as a set of macros for changing between devices.

/// https://github.com/yan0lovesha/AudioSwitch seems to do the same job but using Rust


#include "audio-dev-enumerate.h"


#include <codecvt>
#include <string>
#include <iostream>
#include <mmdeviceapi.h>

#include <endpointvolume.h>

#include <Functiondiscoverykeys_devpkey.h>

// This contains the magic of changing the devices as most Googles would find IMMDeviceEnumerator :: EnumAudioEndpoints for finding the list of devices but NOT be able to set the default device!! (frickin Microsoft..)
// You can see the below header being used in a number of projects - e.g. see https://stackoverflow.com/questions/57778069/how-can-i-programmatically-set-the-default-input-and-output-audio-device-for-an
#include "PolicyConfig.h"

// Ok I looked for the simplest command argument parser and this is the one I chose - for better or for worse..
#include "tclap/CmdLine.h"

using namespace TCLAP;

// Ok ok, this is likely not the correct or modern way of going from string to wstring but quite frankly I detest the amount of work it takes to do boiler plate code.
// I only needed this as PolicyConfig.h interfaces use PCWSTR and I prefer simple std::string.
std::wstring utf8ToUtf16(const std::string& utf8Str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.from_bytes(utf8Str);
}

// Here is the important function for changing the active / current device - note it takes a device id (somthing that looks like "{0.0.0.00000000}.{e774d123-a2b9-4445-b182-5b1d3632c0d8}")
HRESULT SetDefaultAudioPlaybackDevice(PCWSTR devID)
{
    IPolicyConfigVista* pPolicyConfig;
    ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
        NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
    if (SUCCEEDED(hr))
    {
        hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
        pPolicyConfig->Release();
    }
    return hr;
}


// Function for enumerating across all available sound devices and then writing the device id and user friendly name to the console.
void EnumerateOutputDevices()
{
    HRESULT hr = S_OK;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDeviceCollection* pCollection = NULL;
    UINT deviceCount = 0;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (SUCCEEDED(hr))
    {
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        if (SUCCEEDED(hr))
        {
            hr = pCollection->GetCount(&deviceCount);
            if (SUCCEEDED(hr))
            {
                for (UINT i = 0; i < deviceCount; i++)
                {
                    IMMDevice* pDevice = NULL;
                    hr = pCollection->Item(i, &pDevice);
                    if (SUCCEEDED(hr))
                    {
                        LPWSTR pwszID = NULL;
                        hr = pDevice->GetId(&pwszID);
                        if (SUCCEEDED(hr))
                        {
                            std::wstring deviceID(pwszID);
                            std::wcout << "Deviceid: " << deviceID << std::endl;

                            IPropertyStore* pProperties = NULL;
                            hr = pDevice->OpenPropertyStore(STGM_READ, &pProperties);
                            if (SUCCEEDED(hr))
                            {
                                PROPVARIANT varName;
                                PropVariantInit(&varName);
                                hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);
                                if (SUCCEEDED(hr))
                                {
                                    std::wcout << "Output device name: " << varName.pwszVal << std::endl;
                                }
                                PropVariantClear(&varName);
                                pProperties->Release();
                            }
                            CoTaskMemFree(pwszID);
                        }
                        pDevice->Release();
                    }
                }
            }
            pCollection->Release();
        }
        pEnumerator->Release();
    }
}



int main(int argc, char* argv[])
{
    CoInitialize(nullptr);

    try {
        // Define the command line object.
        CmdLine cmd("Simple app for changing the default sound device", ' ', "0.9");


        ValueArg<std::string> deviceIdArg("d", "device", "Device Id", false, "null","device id ");
        cmd.add(deviceIdArg);

        TCLAP::SwitchArg listAllSwitch("l", "list", "Show devices", cmd, false);

        // Parse the args.
        cmd.parse(argc, argv);

        // Get the value parsed by each arg.        
        std::wstring name = utf8ToUtf16(deviceIdArg.getValue());

        if (listAllSwitch)
        {
            EnumerateOutputDevices();
        }
        else if (name != L"null")
        {
            //SetDefaultAudioPlaybackDevice(L"{0.0.0.00000000}.{cf8907d5-be32-429b-abf4-fea8d2a6799a}");
            SetDefaultAudioPlaybackDevice(name.c_str());
        }

    }
    catch (ArgException& e)  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }


    
    return 0;
}
