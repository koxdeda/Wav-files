
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

unsigned int rnd()
{
    static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
    unsigned int b;
    b = ((z1 << 6) ^ z1) >> 13;
    z1 = ((z1 & 4294967294U) << 18) ^ b;
    b = ((z2 << 2) ^ z2) >> 27;
    z2 = ((z2 & 4294967288U) << 2) ^ b;
    b = ((z3 << 13) ^ z3) >> 21;
    z3 = ((z3 & 4294967280U) << 7) ^ b;
    b = ((z4 << 3) ^ z4) >> 12;
    z4 = ((z4 & 4294967168U) << 13) ^ b;
    return (z1 ^ z2 ^ z3 ^ z4);
}

struct wavheader
{
	char RIFF[4];
	unsigned long chunkSize;
	char WAVE[4];
	char fmt_[4];
	unsigned long subchunk1Size;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	char data[4];
	unsigned long subchunk2Size;
};

void merge_wav(wavheader header, char * sample)
{
    char filename[256];
    cin.getline(filename, 256);
    fstream head, merged;
    wavheader header_head;
    head.open(filename, fstream::in | fstream::binary);
    if (!head)
    {
        cout << "Invalid file name!" << endl;
        return;
    }
    merged.open("Merged.wav", fstream::out | fstream::binary);
    head.read((char*)&header_head, 44);
    char * sample_head = new char[header_head.subchunk2Size];
    head.read((char*)&sample_head[0], header_head.subchunk2Size);
    if (header.numChannels != header_head.numChannels || header.blockAlign != header_head.blockAlign)
    {
        cout << "Error!" << endl;
        return;
    }
    header_head.subchunk2Size += header.subchunk2Size;
    header_head.chunkSize = 36 + header_head.subchunk2Size;
    merged.write((char*)&header_head, 44);
    merged.write((char*)&sample_head[0], header_head.subchunk2Size - header.subchunk2Size);
    merged.write((char*)&sample[0], header.subchunk2Size);
}

int absolute(int x)
{
    if (x < 0)
        x -= x * 2;
    return x;
}

double absolute(double x)
{
    if (x < 0)
        x -= x * 2;
    return x;
}

void create_sine_wave(float Period, float Amplitude, wavheader Header, int * Samples)
{
    ofstream Wave;
    Wave.open("C:\\Users\\User\\Desktop\\Sine.wav", fstream::binary | fstream::trunc);
    Wave.write((char*)&Header, 44);
    int Sample;
    int l = pow(2, Header.bitsPerSample) / 2;
    for (float i = 0; i < Header.subchunk2Size; i++)
    {
        Sample = sin((i*0.0174533)*Period)* Amplitude*l;
        Wave.write((char*)&Sample, Header.blockAlign);
        Samples[int(i)] = Sample;
    }
}



void create_square_wave(float Period, float Amplitude, wavheader Header, int *Samples)
{
    ofstream Wave;
    Wave.open("C:\\Users\\User\\Desktop\\Square.wav", fstream::binary | fstream::trunc);
    Wave.write((char*)&Header, 44);
    int Sample;
    int l = pow(2, Header.bitsPerSample) / 2;
    for (float i = 0; i < Header.subchunk2Size; i++)
    {
        if (sin((i*0.0174533)*Period) < 0 && absolute(sin((i*0.0174533)*Period)) > 0.01)
            Sample = -l * Amplitude;
        else if (sin((i*0.0174533)*Period)>0 && absolute(sin((i*0.0174533)*Period)) > 0.01)
            Sample = l * Amplitude;
        else
            Sample = 0;
        Wave.write((char*)&Sample, Header.blockAlign);
         Samples[int(i)] = Sample;
    }
}

void Decim(wavheader & Header, int *Samples)
{
    Samples[Header.subchunk2Size] = 0;
    Header.subchunk2Size /= 2;
    Header.chunkSize = 36 + Header.subchunk2Size;
    ofstream Wave;
    Wave.open("C:\\Users\\User\\Desktop\\Square.wav", fstream::binary | fstream::trunc);
    Wave.seekp(0);
    Wave.write((char*)&Header, 44);
    for (int i = 0; i < Header.subchunk2Size *2; i+=2)
    {
        Wave.write((char*)&Samples[i], Header.blockAlign);
    }
}

void quant(wavheader & Header, int *Samples)
{
    ofstream Wave;
    Wave.open("C:\\Users\\User\\Desktop\\Square.wav", fstream::binary | fstream::trunc);
    int y = Header.bitsPerSample;
    cout << "Уровень квантования: ";
    cin >> Header.bitsPerSample;
    cout << endl;
    Header.byteRate = Header.sampleRate*(Header.bitsPerSample / 8);
    Header.blockAlign = Header.bitsPerSample / 8;
    Wave.write((char*)&Header, 44);
    float lx = pow(2, Header.bitsPerSample) / 2;
    float ly = pow(2, y) / 2;
    int Sample;
    float x = ly / lx;
    for (int i = 0; i < Header.subchunk2Size / Header.blockAlign; i++)
    {
        Sample = Samples[i] / x;
        Wave.write((char*)&Sample, Header.blockAlign);
    }
}

int main()
{
    setlocale(LC_ALL,"rus");
    /*
        char filename[256];
        cout << "Введите имя файла:" << endl;
        cin.getline(filename, 256);
        fstream wav;
        wav.open(filename, fstream::in | fstream::binary);
        if (!wav)
        {
            cout << "Неверное имя файла!" << endl;
            main();
        }
        wavheader header;
        wav.read((char*)&header, 44);
        char * sample = new char[header.subchunk2Size];
        wav.read((char*)&sample[0], header.subchunk2Size);


                merge_wav(header, &sample[0]);


        cout << "Выполнено." << endl;

    */



    wavheader Header;
    float Period;
    float Amplitude;
    cout << "Часчтота дискретизации: ";
    cin >> Header.sampleRate;
    cout << endl << "Битов в сэмпле: ";
    cin >> Header.bitsPerSample;
    Header.byteRate = Header.sampleRate*Header.bitsPerSample / 8;
    Header.blockAlign = Header.bitsPerSample / 8;
    cout << endl << "Длительность: ";
    cin >> Header.subchunk2Size;
    Header.subchunk2Size *= Header.byteRate;
    Header.chunkSize = 36 + Header.subchunk2Size;
    cout << endl << "Период: ";
    cin >> Period;
    cout << endl << "Амплитуда: ";
    cin >> Amplitude;
    cout << endl;
    int * Samples = new int[Header.subchunk2Size];



        //create_sine_wave(Period, Amplitude, Header, Samples);

     create_square_wave(Period, Amplitude, Header, Samples);



       quant(Header, Samples);
       Decim(Header, Samples);

}
