#include <cstdio>
#include <cstdint>

namespace binary{
	template<class T> struct settype{
		settype(){}
	};

	struct setoffset{
		int offset;
		setoffset(int offset): offset(offset) {}
	};

	class ostream{
		char *addr;
		int offset;
	public:
		ostream(char *addr): addr(addr), offset(0) {}
		template<class T> ostream &operator<<(const T &val){
			*reinterpret_cast<T *>(addr + offset) = val;
			offset += sizeof(T);
			return *this;
		}
		template<int N> ostream &operator<<(const char (&val)[N]){
			for(int i = 0; i < N - 1; ++i) addr[offset + i] = val[i];
			offset += N - 1;
			return *this;
		}

		template<class T>
		class ostream_type{
			ostream &os;
		public:
			ostream_type(ostream &os): os(os) {}
			ostream_type &operator<<(const T &val){
				os << val;
				return *this;
			}
		};

		template<class T> ostream_type<T> operator<<(settype<T>){
			return ostream_type<T>(*this);
		}

		ostream &operator<<(const setoffset &of){
			offset = of.offset;
			return *this;
		}
	};

	class istream{
		const char *addr;
		int offset;
	public:
		istream(const char *addr): addr(addr), offset(0) {}

		template<class T> T peek(){
			return *reinterpret_cast<const T *>(addr + offset);
		}

		template<class T> T get(){
			T ret = peek<T>();
			offset += sizeof(T);
			return ret;
		}

		template<class T> istream &operator>>(T &dest){
			dest = *reinterpret_cast<const T *>(addr + offset);
			offset += sizeof(T);
			return *this;
		}
		template<int N> istream &operator>>(char (&dest)[N]){
			for(int i = 0; i < N - 1; ++i) dest[i] = addr[offset + i];
			dest[N - 1] = 0;
			offset += N - 1;
			return *this;
		}

		istream &operator<<(const setoffset &of){
			offset = of.offset;
			return *this;
		}
	};
}

int main(){
	char *buf = new char[100];

	binary::ostream os(buf);
	os << binary::settype<int16_t>() << 65 << 66 << 67 << 68;
	os << "ABCD" << 65 << 66 << 67 << 68;

	for(int i = 0; i < 30; ++i) printf("%2d %d %c\n", i, buf[i], buf[i]);

	os << binary::setoffset(8) << "EFGH";

	for(int i = 0; i < 30; ++i) printf("%2d %d %c\n", i, buf[i], buf[i]);

	binary::istream is(buf);
	int16_t a, b, c, d;
	char str[5];
	is >> a >> b >> c >> d >> str;
	printf("%d %d %d %d %s\n", a, b, c, d, str);
	printf("%d ", is.peek<int>());
	printf("%d ", is.get<int>());
	printf("%d ", is.get<int>());
	printf("%d ", is.get<int>());
	printf("%d\n", is.get<int>());

	delete[] buf;
}
