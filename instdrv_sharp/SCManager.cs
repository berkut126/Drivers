/*
* This is a personal academic project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
*/
using System;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.ServiceProcess;
using System.Text;
using Microsoft.Win32.SafeHandles;

namespace instdrv_sharp
{

    public class ScManager
    {

        [DllImport("Advapi32.dll", SetLastError = true)]
        private static extern IntPtr OpenSCManagerW(string lpMachineName,
            [MarshalAs(UnmanagedType.LPWStr)] string lpDatabaseName, uint access);

        [DllImport("Advapi32.dll", SetLastError = true)]
        private static extern IntPtr CreateServiceW(IntPtr hScManager,
            [MarshalAs(UnmanagedType.LPWStr)] string lpServiceName,
            [MarshalAs(UnmanagedType.LPWStr)] string lpDisplayName,
            uint dwDesiredAccess,
            uint dwServiceType,
            uint dwStartType,
            uint dwErrorControl,
            [MarshalAs(UnmanagedType.LPWStr)] string lpBinaryPathName,
            [MarshalAs(UnmanagedType.LPWStr)] string lpLoadOrderGroup,
            [MarshalAs(UnmanagedType.LPWStr)] string lpdwTagId,
            [MarshalAs(UnmanagedType.LPWStr)] string lpDependencies,
            [MarshalAs(UnmanagedType.LPWStr)] string lpServiceStartName,
            [MarshalAs(UnmanagedType.LPWStr)] string lpPassword);

        [DllImport("Advapi32.dll", SetLastError = true)]
        private static extern bool CloseServiceHandle(IntPtr hScManager);

        [DllImport("Advapi32.dll", SetLastError = true)]
        private static extern bool DeleteService(IntPtr hService);

        [DllImport("Advapi32.dll", SetLastError = true)]
        private static extern IntPtr OpenServiceW(
            IntPtr hScManager,
            [MarshalAs(UnmanagedType.LPWStr)] string lpServiceName,
            uint dwDesiredAccess
        );

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern SafeFileHandle CreateFile(string lpFileName, uint dwDesiredAccess,
            uint dwShareMode, IntPtr lpSecurityAttributes, uint dwCreationDisposition,
            uint dwFlagsAndAttributes, IntPtr hTemplateFile);

        [DllImport("Kernel32.dll", SetLastError = true)]
        private static extern bool DeleteFile(
            [MarshalAs(UnmanagedType.LPWStr)] string lpFileName
        );

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        private static extern bool DeviceIoControl(
            SafeFileHandle hDevice,
            uint dwIoControlCode,
            [MarshalAs(UnmanagedType.LPWStr)] string lpInBuffer,
            uint nInBufferSize,
            IntPtr lpOutBuffer,
            uint nOutBufferSize,
            out uint lpBytesReturned,
            IntPtr lpOverlapped);


        private readonly IntPtr _scManager;

        public delegate void PrintDelegate(string a);

        private readonly PrintDelegate _printDelegate;

        private void Print(string a)
        {
            _printDelegate(a);
        }

        public ScManager(PrintDelegate handler)
        {
            _printDelegate = handler;
            _scManager = OpenSCManagerW(null, "ServicesActive", 2);
            if (_scManager != IntPtr.Zero) return;
            var error = new Win32Exception();
            Print(error.Message);
        }

        ~ScManager()
        {
            CloseServiceHandle(_scManager);
        }

        public bool InstallDriver(string path, string name)
        {
            var service = CreateServiceW(_scManager, name, name, 0xF01FF, 1, 3, 1, path, null, null, null, null, null);
            Print($"Creating service {name}.");
            if (service == IntPtr.Zero)
            {
                var error = new Win32Exception();
                Print(error.Message);
                return false;
            }

            Print($"Created service {name}.");
            CloseServiceHandle(service);
            return true;
        }

        public bool DeleteDriver(string name)
        {

            // Open service manager
            var service = OpenServiceW(_scManager, name, 0xF01FF);

            Print("Opening service");

            if (service == IntPtr.Zero)
            {
                var error = new Win32Exception();
                Print(error.Message);
                return false;
            }

            Print("Deleting service");

            // Delete service
            if (DeleteService(service) == false)
            {
                var error = new Win32Exception();
                Print($"Failed to delete service {name} with error {error.Message}");
                return false;
            }

            Print("Closing handle");

            CloseServiceHandle(service);

            return true;

        }

        public bool RunDriver(string name)
        {

            try
            {
                Print("Opening service");

                var service = new ServiceController(name);

                Print($"Starting service {name}.");

                service.Start();

                Print($"Service {name} started.");

                return true;

            }
            catch (Exception e)
            {
                Print(e.Message);
                return false;
            }

        }

        public bool StopService(string name)
        {
            try
            {
                Print("Opening service");

                var service = new ServiceController(name);

                Print($"Stopping service {name}.");

                service.Stop();

                Print($"Service {name} stopped.");

                return true;

            }
            catch (Exception e)
            {
                Print(e.Message);
                return false;
            }
        }

        public bool OpenDevice(string symlink, out SafeFileHandle file)
        {
            try
            {
                Print($"Opening {symlink}");
                file = CreateFile(
                    symlink,
                    0x40000000 | 0x80000000,
                    0,
                    IntPtr.Zero,
                    3,
                    0,
                    IntPtr.Zero
                );
                if (file.IsInvalid)
                {
                    var error = new Win32Exception();
                    Print(error.Message);
                    return false;
                }

                Print($"{symlink} opened");
                return true;
            }
            catch (Exception e)
            {
                Print(e.Message);
                file = null;
                return false;
            }
        }

        public void CloseDevice(ref SafeFileHandle[] handles, string file)
        {
            foreach (var handle in handles)
            {
                handle.Dispose();
            }

            DeleteFile(file);
        }

        public void CloseDevice(ref SafeFileHandle handle, string file)
        {

            handle.Dispose();

            DeleteFile(file);

        }

        public bool Read(string symlink, ref byte[] data, int size)
        {
            try
            {
                var file = CreateFile(
                    symlink,
                    0x80000000,
                    0,
                    IntPtr.Zero,
                    3,
                    0,
                    IntPtr.Zero
                );
                if (file.IsInvalid)
                {
                    file.Dispose();
                    var error = new Win32Exception();
                    Print(error.Message);
                    return false;
                }

                Print(file.IsInvalid.ToString());
                var fileStream = new FileStream(file, FileAccess.Read);
                Print("Writing data.");
                fileStream.Read(data, 0, size);
                Print("Data read.");
                fileStream.Dispose();
                file.Dispose();
                return true;
            }
            catch (Exception e)
            {
                Print(e.Message);
                return false;
            }
        }

        public bool Write(string symlink, ref byte[] data, int size)
        {
            try
            {
                var file = CreateFile(
                    symlink,
                    0x40000000,
                    0,
                    IntPtr.Zero,
                    3,
                    0,
                    IntPtr.Zero
                );
                if (file.IsInvalid)
                {
                    file.Dispose();
                    var error = new Win32Exception();
                    Print(error.Message);
                    return false;
                }

                Print(file.IsInvalid.ToString());
                var fileStream = new FileStream(file, FileAccess.Write);
                Print("Writing data.");
                fileStream.Write(data, 0, size);
                Print("Data written.");
                fileStream.Dispose();
                file.Dispose();
                return true;
            }
            catch (Exception e)
            {
                Print(e.Message);
                return false;
            }
        }

        public bool SendCodes(string symlink, uint code, ref byte[] outBuf, in string inData)
        {
            try
            {
                var file = CreateFile(
                    symlink,
                    0x40000000,
                    0,
                    IntPtr.Zero,
                    3,
                    0,
                    IntPtr.Zero
                );
                if (file.IsInvalid)
                {
                    file.Dispose();
                    var error = new Win32Exception();
                    Print(error.Message);
                    return false;
                }

                var outData = Marshal.AllocHGlobal(Marshal.SizeOf(outBuf[0]) * outBuf.Length);

                Print("Sending code.");
                DeviceIoControl(file, code, inData, (uint)inData.Length * 2 + 2, outData, (uint)outBuf.Length, out var length, IntPtr.Zero);
                Print($"Read {length} bytes.");
                Print("Code sent.");
                Marshal.Copy(outData, outBuf, 0, outBuf.Length);
                Marshal.FreeHGlobal(outData);
                return true;
            }
            catch (Exception e)
            {
                Print(e.Message);
                return false;
            }
        }

    }

}
