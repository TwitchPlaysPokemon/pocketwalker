#include "set.h"
#include "../cpu.h"

InstructionSet::InstructionSet() :
    root(
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.aH()); },
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.aL()); }
    )
{
    root.Add(0x0, 0xC, {
        "MOV.B Rs, Rd",
        2,
        {1, 0, 0, 0, 0, 0},
        [](CPU& cpu)
        {
            const uint8_t* rs = cpu.reg.Reg8(cpu.bH());
            uint8_t* rd = cpu.reg.Reg8(cpu.bL());

            *rd = *rs;
            cpu.reg.MovFlags(*rd);
        }
    });

    root.Add(0x1, 0xD, {
        "CMP.W Rs, Rd",
        2,
        {1, 0, 0, 0, 0, 0},
        [](CPU& cpu)
        {
            const uint16_t* rs = cpu.reg.Reg16(cpu.bH());
            const uint16_t* rd = cpu.reg.Reg16(cpu.bL());

            cpu.reg.SubFlags(*rd, *rs);
        }
    });

    root.Add(0x3, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF}, {
        "MOV.B Rs, @aa:8",
        2,
        {1, 0, 0, 1, 0, 0},
        [](CPU& cpu)
        {
            const uint8_t* rs = cpu.reg.Reg8(cpu.aL());
            const uint8_t abs = cpu.b();

            const uint16_t address = 0xFF00 | abs;

            cpu.mem->Write8(address, *rs);
            cpu.reg.MovFlags(*rs);
        }

    });

    root.Add(0x4, 0x0, {
        "BRA d:8",
        2,
        {2, 0, 0, 0, 0, 0},
        [](CPU& cpu)
        {
            const auto disp = static_cast<int8_t>(cpu.b());
            cpu.reg.PC += disp;
        }
    });

    root.Add(0x4, 0x4, {
        "BCC d:8",
        2,
        {2, 0, 0, 0, 0, 0},
        [](CPU& cpu)
        {
            const auto disp = static_cast<int8_t>(cpu.b());
            if (!cpu.reg.flags.C)
                cpu.reg.PC += disp;
        }
    });

    root.Add(0x4, 0x5, {
        "BCS d:8",
        2,
        {2, 0, 0, 0, 0, 0},
        [](CPU& cpu)
        {
            const auto disp = static_cast<int8_t>(cpu.b());
            if (cpu.reg.flags.C)
                cpu.reg.PC += disp;
        }
    });

    root.Add(0x5, 0x4, {
        "RTS",
        2,
        {2, 0, 1, 0, 0, 2},
        [](CPU& cpu)
        {
            cpu.reg.PC = cpu.Pop16();
        },
        true
    });

    root.Add(0x5, 0x5, {
        "BSR d:8",
        2,
        {2, 0, 1, 0, 0, 0},
        [](CPU& cpu)
        {
            cpu.Push16(cpu.reg.PC);

            const auto disp = static_cast<int8_t>(cpu.b());
            cpu.reg.PC += disp;
        }
    });

    root.Add(0x5, 0xE, {
        "JSR @aa:24",
        4,
        {2, 0, 1, 0, 0, 2},
        [](CPU& cpu)
        {
            cpu.Push16(cpu.reg.PC + 4);

            const uint32_t abs = (cpu.b() << 16) | cpu.cd();
            cpu.reg.PC = abs;
        },
        true
    });


    root.AddSubtable(0x6, 0x8,
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.a()); },
        [](const CPU& cpu) { return static_cast<uint32_t>((cpu.bH() >> 3) & 1); },
        [](InstructionTable& table)
        {
            table.Add(0x68, 0x00, {
                "MOV.B @ERs, Rd",
                2,
                {1, 0, 0, 1, 0, 0},
                [](CPU& cpu)
                {
                    const uint32_t* ers = cpu.reg.Reg32(cpu.bH());
                    uint8_t* rd = cpu.reg.Reg8(cpu.bL());

                    *rd = cpu.mem->Read8(*ers);
                    cpu.reg.MovFlags(*rd);
                }
            });

            table.Add(0x68, 0x01, {
                "MOV.B Rs, @ERd",
                2,
                {1, 0, 0, 1, 0, 0},
                [](CPU& cpu)
                {
                    const uint32_t* erd = cpu.reg.Reg32(cpu.bH());
                    const uint8_t* rs = cpu.reg.Reg8(cpu.bL());

                    cpu.mem->Write8(*erd, *rs);
                    cpu.reg.MovFlags(*rs);
                }
            });
        }
    );

    root.AddSubtable(0x6, 0xA,
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.a()); },
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.bH()); },
        [](InstructionTable& table)
        {
            table.Add(0x6A, 0x00, {
                "MOV.B @aa:16, Rd",
                4,
                {2, 0, 0, 1, 0, 0},
                [](CPU& cpu)
                {
                    const uint16_t abs = cpu.cd();
                    uint8_t* rd = cpu.reg.Reg8(cpu.bL());

                    *rd = cpu.mem->Read8(abs);
                    cpu.reg.MovFlags(*rd);
                }
            });

            table.Add(0x6A, 0x08, {
                "MOV.B Rs, @aa:16",
                4,
                {2, 0, 0, 1, 0, 0},
                [](CPU& cpu)
                {
                    const uint16_t abs = cpu.cd();
                    const uint8_t* rs = cpu.reg.Reg8(cpu.bL());

                    cpu.mem->Write8(abs, *rs);
                    cpu.reg.MovFlags(*rs);
                }
            });
        }
    );

    root.AddSubtable(0x6, 0xC,
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.a()); },
        [](const CPU& cpu) { return static_cast<uint32_t>((cpu.bH() >> 3) & 1); },
        [](InstructionTable& table)
        {
            table.Add(0x6C, 0x00, {
                "MOV.B @ERs+, Rd",
                2,
                {1, 0, 0, 1, 0, 2},
                [](CPU& cpu)
                {
                    uint32_t* ers = cpu.reg.Reg32(cpu.bH());
                    uint8_t* rd = cpu.reg.Reg8(cpu.bL());

                    *rd = cpu.mem->Read8(*ers);
                    cpu.reg.MovFlags(*rd);

                    *ers += 1;
                }
            });

            table.Add(0x6C, 0x01, {
                "MOV.B Rs, @-ERd",
                2,
                {1, 0, 0, 1, 0, 2},
                [](CPU& cpu)
                {
                    uint32_t* erd = cpu.reg.Reg32(cpu.bH());
                    const uint8_t* rs = cpu.reg.Reg8(cpu.bL());

                    *erd -= 1;

                    cpu.mem->Write8(*erd, *rs);
                    cpu.reg.MovFlags(*rs);
                }
            });
        });


    root.AddSubtable(0x6, 0xD,
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.a()); },
        [](const CPU& cpu) { return static_cast<uint32_t>((cpu.bH() >> 3) & 1); },
        [](InstructionTable& table)
        {
            table.Add(0x6D, 0x00, {
                "MOV.W @ERs+, Rd",
                2,
                {1, 0, 0, 0, 1, 2},
                [](CPU& cpu)
                {
                    uint32_t* ers = cpu.reg.Reg32(cpu.bH());
                    uint16_t* rd = cpu.reg.Reg16(cpu.bL());

                    *rd = cpu.mem->Read16(*ers);
                    cpu.reg.MovFlags(*rd);

                    *ers += 2;
                }
            });

            table.Add(0x6D, 0x01, {
                "MOV.W Rs, @-ERd",
                2,
                {1, 0, 0, 0, 1, 2},
                [](CPU& cpu)
                {
                    uint32_t* erd = cpu.reg.Reg32(cpu.bH());
                    const uint16_t* rs = cpu.reg.Reg16(cpu.bL());

                    *erd -= 2;

                    cpu.mem->Write16(*erd, *rs);
                    cpu.reg.MovFlags(*rs);
                }
            });
        });

    root.Add(0x7, 0x7, {
        "BLD #xx:3, Rd",
        2,
        {1, 0, 0, 0, 0, 0},
        [](CPU& cpu)
        {
            const uint8_t imm = cpu.bH();
            const uint8_t* rd = cpu.reg.Reg8(cpu.bL());

            cpu.reg.flags.C =(*rd >> imm) & 1;
        }
    });

    root.AddSubtable(0x7, 0x9,
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.a()); },
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.bH()); },
        [](InstructionTable& table)
        {
            table.Add(0x79, 0x00, {
                "MOV.W #xx:16, Rd",
                4,
                {1, 0, 0, 0, 2, 2},
                [](CPU& cpu)
                {
                    const uint16_t imm = cpu.cd();
                    uint16_t* rd = cpu.reg.Reg16(cpu.bL());

                    *rd = imm;
                    cpu.reg.MovFlags(imm);
                }
            });
        });

    root.AddSubtable(0x7, 0xD,
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.ab()) << 4 | cpu.cH(); },
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.cL()); },
        [](InstructionTable& table)
        {
            table.AddPattern(0x7D007, 0xFF0FF, 0x00, 0xFF, {
                "BSET #xx:3, @ERd",
                4,
                {2, 0, 0, 2, 0, 0},
                [](CPU& cpu)
                {
                    const uint32_t* erd = cpu.reg.Reg32(cpu.bH());
                    const uint8_t imm = cpu.dH();

                    cpu.mem->Write8(*erd, cpu.mem->Read8(*erd) | (1 << imm));
                }
            });
        });

    root.AddSubtable(0x7, 0xF,
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.ab()) << 4 | cpu.cH(); },
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.cL()); },
        [](InstructionTable& table)
        {
            table.AddPattern(0x7F007, 0xFF00F, 0x00, 0xFF, {
                "BSET #xx:3, @aa:8",
                4,
                {2, 0, 0, 2, 0, 0},
                [](CPU& cpu)
                {
                    const uint8_t abs = cpu.b();
                    const uint8_t imm = cpu.dH();

                    const uint16_t address = 0xFF00 | abs;

                    cpu.mem->Write8(address, cpu.mem->Read8(address) | (1 << imm));
                }
            });

            table.AddPattern(0x7F007, 0xFF00F, 0x02, 0xFF, {
                "BCLR #xx:3, @aa:8",
                4,
                {2, 0, 0, 2, 0, 0},
                [](CPU& cpu)
                {
                    const uint8_t abs = cpu.b();
                    const uint8_t imm = cpu.dH();

                    const uint16_t address = 0xFF00 | abs;

                    cpu.mem->Write8(address, cpu.mem->Read8(address) & ~(1 << imm));
                }
            });
        });

    root.AddSubtable(0x0, 0xB,
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.a()); },
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.bH()); },
        [](InstructionTable& table)
        {
            table.Add(0x0B, 0x00, {
                "ADDS #1, ERd",
                2,
                {1, 0, 0, 0, 0, 0},
                [](CPU& cpu)
                {
                    uint32_t* erd = cpu.reg.Reg32(cpu.bL());
                    *erd += 1;
                }
            });
        });

    root.AddSubtable(0x1, 0xB, [](const CPU& cpu) { return static_cast<uint32_t>(cpu.a()); },
        [](const CPU& cpu) { return static_cast<uint32_t>(cpu.bH()); },
        [](InstructionTable& table)
        {
            table.Add(0x1B, 0x08, {
                "SUBS #2, ERd",
                2,
                {1, 0, 0, 0, 0, 0},
                [](CPU& cpu)
                {
                    uint32_t* erd = cpu.reg.Reg32(cpu.bL());
                    *erd -= 2;
                }
            });

            table.Add(0x1B, 0x09, {
                "SUBS #4, ERd",
                2,
                {1, 0, 0, 0, 0, 0},
                [](CPU& cpu)
                {
                    uint32_t* erd = cpu.reg.Reg32(cpu.bL());
                    *erd -= 4;
                }
            });
        });



    root.Add(0xE, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF}, {
        "AND.B #xx:8, Rd",
        2,
        {1, 0, 0, 0, 0, 0},
        [](CPU& cpu)
        {
            const uint8_t imm = cpu.b();
            uint8_t* rd = cpu.reg.Reg8(cpu.aL());

            *rd &= imm;
            cpu.reg.MovFlags(*rd);
        }
    });

    root.Add(0xF, {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF}, {
        "MOV.B #xx:8, Rd",
        2,
        {1, 0, 0, 0, 0, 0},
        [](CPU& cpu)
        {
            const uint8_t imm = cpu.b();
            uint8_t* rd = cpu.reg.Reg8(cpu.aL());

            *rd = imm;
            cpu.reg.MovFlags(*rd);
        }
    });
}

const Instruction* InstructionSet::Decode(const CPU& cpu) const
{
    return root.Decode(cpu);
}
