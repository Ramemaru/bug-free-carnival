#include <stdio.h>
#include <string.h>
#include<stdlib.h>

//構造体の構成
struct Node {
    char name[32];
    int number;
    struct Node* next;
    struct Node* prev;
};

//略された月表記を整数にして返す
int month_num(char *mon) {
    if (strcmp(mon, "Jan") == 0) {
        return 1;
    }
    else if (strcmp(mon, "Feb") == 0) {
        return 2;
    }
    else if (strcmp(mon, "Mar") == 0) {
        return 3;
    }
    else if (strcmp(mon, "Apr") == 0) {
        return 4;
    }
    else if (strcmp(mon, "May") == 0) {
        return 5;
    }
    else if (strcmp(mon, "Jun") == 0) {
        return 6;
    }
    else if (strcmp(mon, "Jul") == 0) {
        return 7;
    }
    else if (strcmp(mon, "Aug") == 0) {
        return 8;
    }
    else if (strcmp(mon, "Sep") == 0) {
        return 9;
    }
    else if (strcmp(mon, "Oct") == 0) {
        return 10;
    }
    else if (strcmp(mon, "Nov") == 0) {
        return 11;
    }
    else if (strcmp(mon, "Dec") == 0) {
        return 12;
    }
    else {
        return 0;
    }


}
//解析をする日付区間を指定する
void analize_range(int* start_y, int* start_m, int* start_d, int* end_y, int* end_m, int* end_d) {
    printf("区間の始めを入力してください\n西暦 月 日で入力:");
    rewind(stdin);
    scanf_s("%d %d %d", start_y, start_m, start_d);
    printf("区間の終わりを入力してください\n西暦 月 日で入力:");
    rewind(stdin);
    scanf_s("%d %d %d", end_y, end_m, end_d);
    if (*end_d - *start_d + 100 * *end_m - 100 * *start_m + 1200 * *end_y - 1200 * *start_y < 0) {
        printf("エラー!区間を正しく指定してください\n\n");
        analize_range(start_y, start_m, start_d, end_y, end_m, end_d);
    }
}
//時間帯の解析を行いテーブルを更新する
void analize_t(int* hash, char time[]) {
    char buf[3];
    buf[0] = time[12]; buf[1] = time[13]; buf[2] = '\0';
    int access_h = atoi(buf);
    hash[access_h] += 1;
}
//リストに要素を追加する
struct Node* addnode(struct Node *node, char *name) {
    struct Node* head;
    struct Node* tail;

    head = node;
    tail = (struct Node*)malloc(sizeof(struct Node));
    tail->number = 1;
    strcpy_s(tail->name, name);
    tail->next = NULL;
    tail->prev = NULL;
    //引数が空リストの場合
    if (head == NULL) {
        return tail;
    }
    //引数が空リストでない場合
    while (node->next != NULL) {
        node = node->next;
    }
    node->next = tail;
    tail->prev = node;
    return head;
}
//ホストの解析を行いリストを更新する
struct Node *analize_h(struct Node *node, char *name) {
    struct Node* head;
    head = node;
    //引数が空リストの場合
    if (node == NULL) {
        node=addnode(node, name);
        return node;
    }
    //引数が空リストでない場合、nameが既に存在するか調べる
    else while (1) {
        if (strcmp(node->name, name) == 0) {
            node->number++;
            //順序がおかしい場合はsortする
            if (node->prev != NULL && node->number > node->prev->number) {
                struct Node *unsorted=node;
                node->prev->next = node -> next;
                if (node->next != NULL) {
                    node->next->prev = node->prev;
                }
                //動かす節点をunsortedとして、挿入する場所の直後までnodeを動かす
                while (node->prev != NULL && unsorted->number > node->prev->number) {
                    node = node->prev;
                }
                unsorted->next = node;
                unsorted->prev = node->prev;
                if (node->prev != NULL) {
                    node->prev->next = unsorted;
                }
                node->prev = unsorted;
                if (unsorted->prev == NULL) {
                    head = unsorted;
                }
            }
            return head;
        }
        node = node -> next;
        if (node == NULL) {
            head=addnode(head, name);
            return head;
        }
    }
}


int main() {
    char quest = 'a';
    char file_input = 'a';
    int start_y;  int end_y;
    int start_m;  int end_m;
    int start_d;  int end_d;
    int start_t;    int end_t;
    int access_t;
    char txt_path[256];
    char log_path[256];

    
    //期間の指定をするか y or n で問い合わせ
    while (quest == 'a') {
        printf("日付区間を指定しますか?\n y or nで入力:");
        rewind(stdin);
        scanf_s("%c", &quest,1);
        if (quest == 'y') {
            printf("日付区間を指定して解析します\n");
            analize_range(&start_y,&start_m,&start_d,&end_y,&end_m,&end_d);
            printf("%d年%d月%d日から%d年%d月%d日まで解析します\n\n", start_y, start_m, start_d, end_y, end_m, end_d);
            break;
        }
        if (quest == 'n') {
            start_y = 1; end_y = 9999;
            start_m = 1; end_m = 12;
            start_d = 1; end_d = 31;
            printf("日付区間を指定せずに解析します\n\n");

            break;
        }
        printf("エラー!yかnで入力してください\n\n");
        quest = 'a';
    }
    //ここまでで日付区間設定完了

    quest = 'a';
    start_t = start_d + start_m * 100 + start_y * 10000;
    end_t = end_d + end_m * 100 + end_y * 10000;

    
    //ファイルを読み込みモードで開く
    FILE* fp=NULL;
    FILE* tp=NULL;
    printf("ファイルを開きます\n");
    printf("/var/log/httpd/access_logを解析する場合はyを、解析するファイルをまとめたテキストを読み込む場合はnを入力してください\n");
    while (file_input == 'a') {
        printf("デフォルトでログを開きますか?\n y or nで入力:");
        rewind(stdin);
        scanf_s("%c", &file_input,1);
        if (file_input == 'y') {
            if ((fopen_s(&fp, "/var/log/httpd/access_log", "r")) != 0) {
                //ファイルオープンに失敗した場合その旨を出力
                printf("エラー!ファイルを開けませんでした");
                return -1;
            }
            strcpy_s(log_path, "/var/log/httpd/access_log");
            printf("%sを開きました\n",log_path);
            
            break;
        }
        if (file_input == 'n') {
            printf("解析したいファイルをまとめたテキストのパスを入力してください:");
            rewind(stdin);
            scanf_s("%s", txt_path,256);
            if ((fopen_s(&tp, txt_path, "r")) != 0) {
                //ファイルオープンに失敗した場合その旨を出力
                printf("エラー!ファイルを開けませんでした");
                return -1;
            }
            printf("%sを開きました\n",txt_path);
            break;
        }
        printf("エラー!yかnで入力してください\n\n");
        file_input = 'a';
    }

    //解析の内容を h or t で指定する
    while (quest == 'a') {
        printf("行いたい解析を選んでください\n\n");
        printf("各時間帯毎のアクセス件数を知りたい(t)\nor\nアクセスの多いリモートホストの順にアクセス件数の一覧を表示する(h)\nt or hで入力:");
        rewind(stdin);
        scanf_s("%c", &quest,1);
        if (quest == 't') {
            printf("各時間帯毎のアクセス件数を解析します\n\n");
            break;
        }
        if (quest == 'h') {
            printf("アクセスの多いリモートホストの順にアクセス件数の一覧を表示します\n\n");
            break;
        }
        printf("エラー!tかhで入力してください\n\n");
        quest = 'a';
    }


    char str[256];
    char host[32] = { 0 };
    char time[32] = { 0 };
    char buf[16] = { 0 };
    struct Node *node = NULL;
    int timetable[24] = { 0 };
    int unreadable = 0;
    
    //解析部分
    while (file_input == 'y' || fgets(log_path, 256, tp) != NULL) {
        //読み込みファイル名から改行削除
        if (log_path[strlen(log_path) - 1] == '\n') {
            log_path[strlen(log_path) - 1] = '\0';
        }
        if ((fopen_s(&fp, log_path, "r")) != 0) {
            unreadable = 1;
            continue;
        }
        while (fgets(str, 256, fp) != NULL) {   //hostとtimeに読み込んだ内容を抜き出し
            strncpy_s(host, str, strchr(str, ' ') - str);
            strncpy_s(time, strchr(str, '[') + 1, strchr(str, ']') - strchr(str, '[') - 1);

            //access_tにアクセス日を整数型で格納
            buf[0] = time[7]; buf[1] = time[8]; buf[2] = time[9]; buf[3] = time[10]; buf[4] = buf[5] = '0'; buf[6] = time[0]; buf[7] = time[1]; buf[8] = '\0';
            access_t = atoi(buf);
            buf[0] = time[3];buf[1] = time[4];buf[2] = time[5];buf[3] = '\0';
            access_t += 100 * month_num(buf);
            //printf("host: %s\n", host);
            //printf("time: %s\n", time);

            //アクセス日が指定区間でなければスキップ
            if (access_t > end_t || access_t < start_t) {
                continue;
            }
            //解析の関数を起動
            if (quest == 'h') {
                node = analize_h(node, host);
            }
            else if (quest == 't') {
                analize_t(timetable, time);
            }

        }
        fclose(fp);
        if (file_input == 'y') {
            break;
        }
    }

    //最終出力
    if (quest == 'h') {
        while (node != NULL) {
            printf("%sのアクセス:%d\n",node->name,node->number);
            node = node->next;
        }
    }
    else if (quest == 't') {
        for (int i = 0;i < 24;i++) {
            printf("%d時から%d時までのアクセス:%d\n", i, i + 1, timetable[i]);
        }
    }

    //ファイルを閉じる
    if (file_input == 'n') {
        fclose(tp);
    }
    if (unreadable == 1) {
        printf("読み込めないファイルがありました\n");
    }
    return 0;
}