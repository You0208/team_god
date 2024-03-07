#include<stdint.h>

template <uint16_t time>//timeに入れた瞬間関数が完成
class interval
{
	uint32_t next_run = 0;

	template <class T>
	void _run(T func)
	{
		uint32_t now = clock();
		if (next_run < now)
		{
			func();
			next_run = now + time;
		}
	}
	interval() {}
public:
	template <class T>
	static void run(T func)//singletonパターン
	{
		static interval<time> instance;
		instance._run(func);
	}
};


//使い方
//#include "interval.h"
//void update()
//{
// 
//			ミリ秒		&を入れると外の変数にアクセスできる
//			↑			↓
// interval<1000>::run([]{
//		<<ここに一秒ごとに死体処理を入れる>>
// なぜラムダ式を使うか→ここにバンと書けるので使うところと書くところが一緒
// });
// 
//}

