#include "sm3.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

// 显示消息框函数
void ShowMessage(const char* title, const char* message) {
    MessageBox(NULL, message, title, MB_OK | MB_ICONINFORMATION);
}

// 对话框过程函数
BOOL CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static char input_buffer[1024];
    static char result_buffer[128];
    unsigned char hash[SM3_DIGEST_SIZE];
    
    switch (msg) {
        case WM_INITDIALOG:
            return TRUE;
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1002: // 计算按钮
                    // 获取输入
                    GetWindowText(GetDlgItem(hwnd, 1001), input_buffer, sizeof(input_buffer));
                    
                    // 计算SM3哈希
                    sm3_hash((unsigned char*)input_buffer, strlen(input_buffer), hash);
                    
                    // 转换为字符串
                    sm3_hash_to_string(hash, result_buffer);
                    
                    // 显示结果
                    SetWindowText(GetDlgItem(hwnd, 1003), result_buffer);
                    break;
                    
                case IDCANCEL: // 退出按钮
                    EndDialog(hwnd, 0);
                    break;
            }
            return TRUE;
    }
    return FALSE;
}

// 简单控制台版本 - 直接进入输入
void ConsoleVersion() {
    char input[1024];
    unsigned char hash[SM3_DIGEST_SIZE];
    char result[65];
    int len;
    
    printf("请输入要计算SM3哈希的字符串: ");
    
    // 使用fgets读取输入
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("读取输入失败！\n");
        return;
    }
    
    // 去除换行符
    len = (int)strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
        len--;
    }
    
    // 计算哈希
    sm3_hash((unsigned char*)input, len, hash);
    sm3_hash_to_string(hash, result);
    
    printf("SM3哈希结果: %s\n", result);
    printf("按Enter键退出...");
    getchar();
}

// 直接启动控制台版本的主函数
int main() {
    // 直接进入控制台版本，不显示菜单
    ConsoleVersion();
    return 0;
}
