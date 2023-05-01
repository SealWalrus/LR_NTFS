// NTFS_Reader.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <iomanip>
#include <windows.h>
#include <string>

using namespace std;

class NTFS
{
private:
    BYTE* BootRecord;
    BYTE* Cluster;
    int len_cluster;
    int ClusterSize;
    bool readcluster=false;
#pragma pack(push,1)
    typedef struct {
        BYTE    jump[3];
        BYTE    name[8];
        UINT16  sec_size;
        BYTE    secs_cluster;
        BYTE    reserved_0[7];
        BYTE    media_desc;
        UINT16  reserved_2;
        UINT16  secs_track;
        UINT16  num_heads;
        BYTE    reserved_3[8];
        UINT16  reserved_4;
        UINT16  reserved_5;
        UINT64  num_secs;
        UINT64  mft_clus;
        UINT64  mft2_clus;
        UINT32  mft_rec_size;
        UINT32  buf_size;
        UINT64  volume_id;
    }BootNtfs;
    BootNtfs* ntfs;
    HANDLE DiskHandle;
public:
    
	NTFS(string path)
	{
        
        DiskHandle = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        BootRecord = new BYTE[1024];
        DWORD BytesRead;
        auto readboot = ReadFile(DiskHandle, BootRecord, 1024, &BytesRead, NULL);
         ntfs= (BootNtfs*)BootRecord;
        ClusterSize = ntfs->secs_cluster * ntfs->sec_size;
        Cluster = new BYTE[ClusterSize];
        

	}
    void GetBootInfo()
    {
        cout << "Файловая система " << ntfs->name<<endl;
        cout << "Размер сектора в байтах " << ntfs->sec_size << endl;
        cout << "Кластерный множитель " << int(ntfs->secs_cluster) << endl;
        cout << "Размер кластера " << ClusterSize << endl;
        cout << "Кластер в котором начинается таблица MFT " << ntfs->mft_clus << endl;
        cout << "Кластер в котором начинается копия таблицы MFT " << ntfs->mft2_clus << endl;
    }
    void ReadCluster(int NumCluster, int CountCluster)
    {   
        len_cluster = CountCluster * ClusterSize;
        if (CountCluster!=1)
        Cluster = new BYTE[len_cluster];
        DWORD BytesRead;
        auto position = SetFilePointer(DiskHandle, NumCluster * ClusterSize, NULL, FILE_BEGIN);
        readcluster = ReadFile(DiskHandle, Cluster, CountCluster * ClusterSize, &BytesRead, NULL);
        switch (readcluster)
        {
        case true:
            cout << "Данные прочитаны успешно!" << endl;
            break;
        default:
                cout << "Данные не были прочитаны!" << endl;
                break;
        }

    }
    void PrintCluster()
    {
        if (readcluster)
        {
            for (int i = 0; i < len_cluster; ++i)
            {
                cout << std::hex << setw(2) << setfill('0') << uppercase << int(Cluster[i]) << ' ';
                if ((i + 1) % 16 == 0)
                    cout << endl;
            }            
        }
        else cout << "Кластер не прочитан! Используйте метод ReadCluster";
    }

	~NTFS()
	{
        delete[] Cluster;
        delete[] BootRecord;
        CloseHandle(DiskHandle);
        #pragma pack(pop)
	}
};


int main()
{
	setlocale(LC_ALL, "rus");

    string path;
    cout << "Введите Букву логического тома из которого необходимо читать данные: " << endl;
    cin >> path;
    path = "\\\\.\\" + path + ":";
    NTFS obj(path);
    obj.GetBootInfo();
    system("pause");
    cout << "С какого кластера нужно начать чтение?" << endl<<"Введите число: ";
    int num, count;
    cin >> num;
    cout << "Сколько кластеров необходимо прочитать?" << endl<<"Введите число: ";
    cin >> count;
    obj.ReadCluster(num, count);
    cout << "Вывести данные в консоль?" << endl << "1 - вывести, 0 - выход" << endl;
    int mode; cin >> mode;
    switch (mode)
    {
    case 1:
        obj.PrintCluster();
        break;
    case 0:
        cout << "Выход из программы..." << endl;
        break;
    default:
        cout << "Нужно было ввести 0 или 1 дубина!" << endl;
        return 1;
    }
    
	system("pause");
	return 0;
}


