#include <iostream>
#include "uptime.h"
#include <stdio.h>

using namespace std;

/** Created a  System Class **/

class SystemAnalyzer{

public:

    /** Get Device Information **/

    string GetInformationFromCommand(string cmdline) {
        string data;
        FILE * stream;
        const int max_buffer = 256;
        char buffer[max_buffer];

        stream = popen(cmdline.c_str(), "r");
        if (stream) {
          while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
              pclose(stream);
          }
        return data;
    }

    void GetDeviceInfo()
    {
        freopen("Device.info", "w", stdout);

        string ItemsName[] = {  "BIOS Version\t\t: ", "BIOS Vendor\t\t: ", "Release Date\t\t: ",
                            "Motherboard Name\t: ", "Motherboard Vendor\t: ", "Motherboard Versions\t: ",
                            "Product\t\t\t: ", "Product Name\t\t: ", "Product Version\t\t: ", "System Vendor\t\t: "};

        string ItemsPath[] = {"/sys/class/dmi/id/bios_version", "/sys/class/dmi/id/bios_vendor", "/sys/class/dmi/id/bios_date",
                            "/sys/class/dmi/id/board_name", "/sys/class/dmi/id/board_vendor", "/sys/class/dmi/id/board_version",
                            "/sys/class/dmi/id/product_sku", "/sys/class/dmi/id/product_name", "/sys/class/dmi/id/product_version", "/sys/class/dmi/id/sys_vendor"};

        for(int i=0; i<10; i++)
        {
            ifstream ifs (ItemsPath[i], ifstream::in);

            string line;
           while(getline(ifs, line)) cout<<ItemsName[i]<<line<<'\n';
           if(i%3==2) cout<<'\n';
        }
        fclose(stdout);
    }

    /** Get Memory Information **/

    void GetMemInfo()
    {
        freopen("Memory.info", "w", stdout);

        string memHWInfo = GetInformationFromCommand("");
        cout<<memHWInfo<<'\n';

        ifstream ifs ("/proc/meminfo", ifstream::in);

        string line;
        int cnt = 0;
        while(getline(ifs, line))
        {
            cout<<line<<'\n';
            if(cnt++>3) break;
        }

        fclose(stdout);
    }

    /** Get Hard Disk Information **/

    void GetHDDInfo()
    {
        freopen("HardDisk.info", "w", stdout);

        string ptype[2] = {"GPT", "MBR"};   /// Hard disk partition style
        string hddNames[] = {"Disk Model\t: ", "Vendor\t\t: ", "Size\t\t: ", "Version\t\t: ", "Partitioned\t: "};  /// hard disk hardware information
        string hddItems[] = {"/sys/block/sda/device/model", "/sys/block/sda/device/vendor", "/sys/block/sda/size", "/sys/block/sda/device/rev", "/sys/block/sda/device/type"};
        string Size[] = {" KB", " MB", " GB", " TB"};

        for(int i=0; i<5; i++)
        {
            ifstream ifs (hddItems[i], ifstream::in);
            string line;
            while(getline(ifs, line))
            {
                if(i==2) {
                    int sz = stoi(line)/2;
                    cout<<hddNames[i]<<sz/1048576<<" GB\n";   /// converted hard disk size in GIGA Bytes
                }
                else if(i==4)
                {
                    int tp = stoi(line);
                    cout<<hddNames[i]<<ptype[tp]<<'\n';
                }
                else
                cout<<hddNames[i]<<line<<'\n';
            }

        }

        cout<<"\nList of Partition in dev/:\n";

        ifstream ifs ("/proc/partitions", ifstream::in);
        string line;
        string fltr = "   8";                              /// finding partitions
        cout<<"\nList \t Logical Name \t Physical Size\n\n";


        while(getline(ifs, line))
        {
            if(!line.compare(0, fltr.size(), fltr))
            {
                string major, minor, block, name;
                stringstream ss(line);
                ss >> major >> minor >> block >> name;

                int sz = stoi(block);
                int cnt = 0;
                while(sz>1024)
                {
                    sz /= 1024;
                    cnt++;
                }
                if(name.length()==3) name+="0";

                cout<<minor<<'\t'<<"dev/"<<name<<'\t'<<sz<<'\t'<<Size[cnt]<<'\n';
            }
        }

        fclose(stdout);
    }

    /** Get Control Processing Unit Information **/

    void GetCPUInfo()
    {
        freopen("CPU.info", "w", stdout);
        ifstream ifs ("/proc/cpuinfo", ifstream::in);

        string items[] = {"processor", "vendor_id", "model name", "cache size", "siblings", "cpu cores"};
        string line;
        int k = 0;

        while(getline(ifs, line))
        {
            if(!line.compare(0, items[0].size(), items[0]))
            {
                k++;
            }
        }

        cout<<"Processor\t: "<<k<<'\n';

        ifstream dfs ("/proc/cpuinfo", ifstream::in);
        k = 1;
        while(getline(dfs, line))
        {

            if(!line.compare(0, items[k].size(), items[k]))
            {
                cout<<line<<endl;
                k++;
            }
            if(k>5) break;
        }

        vector<CPUData> entries1;
        vector<CPUData> entries2;

        ReadStatsCPU(entries1);

        this_thread::sleep_for(chrono::milliseconds(100));

        ReadStatsCPU(entries2);

	    cout<<"\nControl Processing Unit Uptime\n\n";
        PrintStats(entries1, entries2);

    }


    /** Get Graphics Processor Unit Information **/

    void GetGPUInfo()
    {
        freopen("GPU.info", "w", stdout);

        string ot = GetInformationFromCommand("lspci");   /// get a list of PCI associate with the PC
        cout<<ot<<'\n';
        ot = GetInformationFromCommand("lspci -v -s 00:02.0");   /// details about VGS Graphics card which is listed in serial as given in this command
        cout<<"\n"<<ot<<'\n';

        fclose(stdout);

    }
};
