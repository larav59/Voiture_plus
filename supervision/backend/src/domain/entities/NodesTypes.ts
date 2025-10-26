import { Column, Entity, OneToMany, PrimaryGeneratedColumn } from "typeorm";
import { Nodes } from "./Nodes";

@Entity("NODES_TYPES", { schema: "supervision" })
export class NodesTypes {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("varchar", { name: "label", nullable: true, length: 255 })
  public label!: string | null;

  @OneToMany(() => Nodes, (nodes) => nodes.nodeType2)
  public nodes!: Nodes[];

  public constructor(init?: Partial<NodesTypes>) {
    Object.assign(this, init);
  }
}
