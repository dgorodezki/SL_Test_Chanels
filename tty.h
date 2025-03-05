#include <windows.h>
#include <codecvt>
#include <locale>
#include <string>

class TTY {
 public:
  TTY() { m_Handle = INVALID_HANDLE_VALUE; }
  virtual ~TTY() { Disconnect(); }

  bool IsOK() const { return m_Handle != INVALID_HANDLE_VALUE; }

  void Connect(const std::string& port, int baudrate) {
    Disconnect();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wport = converter.from_bytes(port);

    m_Handle = CreateFile(wport.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                          OPEN_EXISTING, 0, NULL);

    if (m_Handle == INVALID_HANDLE_VALUE) {
      return;
    }

    COMMTIMEOUTS CommTimeOuts{};
    CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
    CommTimeOuts.ReadTotalTimeoutConstant = 0;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
    CommTimeOuts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(m_Handle, &CommTimeOuts)) {
      Disconnect();
    }

    DCB ComDCM{};
    memset(&ComDCM, 0, sizeof(ComDCM));
    ComDCM.DCBlength = sizeof(DCB);
    GetCommState(m_Handle, &ComDCM);
    ComDCM.BaudRate = DWORD(baudrate);
    ComDCM.ByteSize = 2;
    ComDCM.Parity = NOPARITY;
    ComDCM.StopBits = ONESTOPBIT;
    ComDCM.fAbortOnError = TRUE;
    ComDCM.fDtrControl = DTR_CONTROL_DISABLE;
    ComDCM.fRtsControl = RTS_CONTROL_DISABLE;
    ComDCM.fBinary = TRUE;
    ComDCM.fParity = FALSE;
    ComDCM.fInX = FALSE;
    ComDCM.fOutX = FALSE;
    ComDCM.XonChar = 0;
    ComDCM.XoffChar = (unsigned char)0xFF;
    ComDCM.fErrorChar = FALSE;
    ComDCM.fNull = FALSE;
    ComDCM.fOutxCtsFlow = FALSE;
    ComDCM.fOutxDsrFlow = FALSE;
    ComDCM.XonLim = 128;
    ComDCM.XoffLim = 128;

    if (!SetCommState(m_Handle, &ComDCM)) {
      Disconnect();
    }
  }

  void Disconnect() {
    if (m_Handle != INVALID_HANDLE_VALUE) {
      CloseHandle(m_Handle);
      m_Handle = INVALID_HANDLE_VALUE;
    }
  }

  void Write(const float data[], int size) {
    DWORD feedback;
    if (!WriteFile(m_Handle, data, sizeof(float) * size, &feedback, 0)) {
      CloseHandle(m_Handle);
      m_Handle = INVALID_HANDLE_VALUE;
    }
  }

 private:
  HANDLE m_Handle;
};
