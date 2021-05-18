typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef volatile struct tag_gpio {
	uint32_t moder;
	uint32_t otyper;
	uint32_t ospeedr;
	uint32_t pupdr;
	union {
	uint32_t idr;
	struct {
		uint8_t idrLow;
		uint8_t idrHigh;
		short reserved;
		};
	};
	union {
	uint32_t odr;
	struct {
		uint8_t odrLow;
		uint8_t odrHigh;
		short reserved2;
		};
	};
	
} GPIO;
#define GPIO_D (*((volatile GPIO*) 0x40020c00))
#define GPIO_E (*((volatile GPIO*) 0x40021000))